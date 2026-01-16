#pragma once
#include <jni.h>
#include <string>
#include "../java/env.h"
#include <algorithm>

namespace TheVoid {
	namespace Utils {
        inline bool FindGameWindow() {
            // 1. Intento por Clase "LWJGL" (Estándar en 1.8.9 / Forge / Vanilla)
            TheVoid::Java::GameWindow = FindWindowA("LWJGL", nullptr);

            if (TheVoid::Java::GameWindow) return true;

            // 2. Intento por Título (Para Lunar Client a veces cambia la clase)
            // Esto busca cualquier ventana que empiece con "Minecraft" o "Lunar"
            // (Es un método un poco sucio pero funciona si falla el primero)
            TheVoid::Java::GameWindow = FindWindowA(nullptr, "Minecraft 1.8.9");
            if (TheVoid::Java::GameWindow) return true;

            return false;
        }

        // =========================================================
        // OBTIENE EL NOMBRE REAL DE UNA CLASE (Reflexión)
        // Equivalente a: object.getClass().getName();
        // Costoso: Usar solo cuando se aprende una clase nueva.
        // =========================================================
        inline std::string GetClassName(jobject obj) {
            if (!obj || !TheVoid::Java::env) return "";
            JNIEnv* env = TheVoid::Java::env;

            // 1. Obtener la clase del objeto
            jclass cls = env->GetObjectClass(obj);
            if (!cls) return "";

            // 2. Obtener la clase 'java.lang.Class'
            jclass classClass = env->FindClass("java/lang/Class");
            if (!classClass) {
                env->DeleteLocalRef(cls);
                return "";
            }

            // 3. Obtener el método 'getName'
            jmethodID midGetName = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
            if (!midGetName) {
                env->DeleteLocalRef(classClass);
                env->DeleteLocalRef(cls);
                return "";
            }

            // 4. Llamar al método -> Devuelve jstring
            jstring strObj = (jstring)env->CallObjectMethod(cls, midGetName);
            if (!strObj) {
                env->DeleteLocalRef(classClass);
                env->DeleteLocalRef(cls);
                return "";
            }

            // 5. Convertir a C++ String
            const char* strChars = env->GetStringUTFChars(strObj, nullptr);
            std::string result(strChars);

            // 6. Limpieza OBLIGATORIA
            env->ReleaseStringUTFChars(strObj, strChars);
            env->DeleteLocalRef(strObj);
            env->DeleteLocalRef(classClass);
            env->DeleteLocalRef(cls);

            return result;
        }

        // =========================================================
        // CHEQUEO RÁPIDO DE INSTANCIA
        // Equivalente a: object instanceof ClassName
        // =========================================================
        inline bool IsInstanceOf(jobject obj, jclass targetClass) {
            if (!obj || !targetClass || !TheVoid::Java::env) return false;
            return TheVoid::Java::env->IsInstanceOf(obj, targetClass);
        }

        inline jclass FindClass(const char* name) {
            JNIEnv* env = TheVoid::Java::env;
            if (!env) return nullptr;

            // 1. Intento Nativo (Rápido, pero suele fallar en hilos externos)
            jclass cls = env->FindClass(name);
            if (cls) return cls;
            env->ExceptionClear(); // Limpiamos el error si falló

            // 2. Intento Manual con ClassLoader (El truco maestro)
            if (TheVoid::Java::ClassLoaderObj) {
                // Convertimos formato: "net/minecraft/..." -> "net.minecraft..."
                std::string className(name);
                std::replace(className.begin(), className.end(), '/', '.');

                // Obtenemos la clase ClassLoader y su método loadClass
                jclass loaderClass = env->GetObjectClass(TheVoid::Java::ClassLoaderObj);
                jmethodID loadClassMid = env->GetMethodID(loaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

                jstring strName = env->NewStringUTF(className.c_str());

                // LLAMADA MÁGICA: loader.loadClass("ave");
                cls = (jclass)env->CallObjectMethod(TheVoid::Java::ClassLoaderObj, loadClassMid, strName);

                // Limpieza
                env->DeleteLocalRef(strName);
                env->DeleteLocalRef(loaderClass);

                if (cls) return cls;
                env->ExceptionClear();
            }

            return nullptr; // Se rindió
        }

        inline std::string GetClassSignature(jclass cls) {
            JNIEnv* env = TheVoid::Java::env;
            jclass classClass = env->GetObjectClass(cls);
            jmethodID getName = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
            jstring nameStr = (jstring)env->CallObjectMethod(cls, getName);

            const char* nameChars = env->GetStringUTFChars(nameStr, 0);
            std::string name(nameChars);
            env->ReleaseStringUTFChars(nameStr, nameChars);
            env->DeleteLocalRef(classClass);

            // Si es un array o primitiva, la lógica cambia, pero para Objetos normales:
            // 1. Reemplazar . por /
            std::replace(name.begin(), name.end(), '.', '/');

            // 2. Si no empieza con [, es un objeto normal, agregamos L y ;
            if (name.find("[") == std::string::npos) {
                return "L" + name + ";";
            }
            return name; // Si ya es firma de array, se deja igual
        }
	}
}