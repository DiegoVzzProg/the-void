#pragma once
#include <jni.h>
#include <string>
#include "../java/env.h"

namespace TheVoid {
	namespace Utils {
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
	}
}