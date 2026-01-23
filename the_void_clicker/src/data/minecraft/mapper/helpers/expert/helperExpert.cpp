#include <unordered_map>
#include <iostream>
#include "../../Mapper.h"
#include "../../../../env.h"
#include "../../../../encrypt/encrypt.h"
#include "../../../../utils/javautils.h"

#include "helperExpert.h"

namespace Helper {
    void Expert::ResolveFieldGeneric(std::string classKey, std::string fieldKey, std::vector<std::string> nameCandidates, std::string expectedTypeKey) {
        // 1. Validar que la clase existe
        if (Java::Mapper::classes.find(classKey)
            == Java::Mapper::classes.end()) {
            std::cout << "[MAPPER ERROR] Clase no registrada: " << classKey << std::endl;
            return;
        }

        std::string className = Java::Mapper::classes[classKey];
        jclass cls = Utils::FindClass(className.c_str());
        if (!cls) { Java::CEnv::env->ExceptionClear(); return; }

        // 2. Obtener TODOS los campos de la clase
        jclass classClass = Java::CEnv::env->GetObjectClass(cls);
        jmethodID getDeclaredFields = Java::CEnv::env->GetMethodID(classClass, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
        jobjectArray fieldsArray = (jobjectArray)Java::CEnv::env->CallObjectMethod(cls, getDeclaredFields);

        if (!fieldsArray) {
            Java::CEnv::env->DeleteLocalRef(cls);
            return;
        }

        jsize count = Java::CEnv::env->GetArrayLength(fieldsArray);

        // Métodos de Reflexión
        jclass fieldClass = Java::CEnv::env->FindClass("java/lang/reflect/Field");
        jmethodID getName = Java::CEnv::env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
        jmethodID getType = Java::CEnv::env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
        jclass classOfClass = Java::CEnv::env->FindClass("java/lang/Class");
        jmethodID getClassName = Java::CEnv::env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

        bool found = false;

        // Resolver el nombre de la clase del tipo esperado (si se dio uno)
        std::string targetType = "";
        if (!expectedTypeKey.empty()
            && Java::Mapper::classes.find(expectedTypeKey)
            != Java::Mapper::classes.end()) {
            targetType = Java::Mapper::classes[expectedTypeKey];
        }

        for (int i = 0; i < count; i++) {
            jobject fieldObj = Java::CEnv::env->GetObjectArrayElement(fieldsArray, i);

            // A) Obtener Nombre del Campo
            jstring nameStr = (jstring)Java::CEnv::env->CallObjectMethod(fieldObj, getName);
            const char* nameChars = Java::CEnv::env->GetStringUTFChars(nameStr, 0);
            std::string currentName(nameChars);
            Java::CEnv::env->ReleaseStringUTFChars(nameStr, nameChars);

            // B) Obtener Tipo del Campo (Nombre de la clase)
            jobject typeObj = Java::CEnv::env->CallObjectMethod(fieldObj, getType);
            jstring typeStr = (jstring)Java::CEnv::env->CallObjectMethod(typeObj, getClassName);
            const char* typeChars = Java::CEnv::env->GetStringUTFChars(typeStr, 0);
            std::string currentType(typeChars);
            Java::CEnv::env->ReleaseStringUTFChars(typeStr, typeChars);

            bool match = false;

            // CRITERIO 1: Coincidencia por Nombre
            for (const auto& cand : nameCandidates) {
                if (currentName == cand) {
                    match = true;
                    break;
                }
            }

            // CRITERIO 2: Coincidencia por Tipo (Plan B)
            // Solo si no coincidió el nombre y tenemos un tipo objetivo configurado
            if (!match && !targetType.empty()) {
                // Verificamos si el tipo del campo es el que buscamos
                // Nota: Usamos find por si es un array "[L..." o nombre directo
                if (currentType.find(targetType) != std::string::npos) {
                    match = true;
                    // Debug opcional: std::cout << "   [SMART] Campo '" << currentName << "' encontrado por TIPO." << std::endl;
                }
            }

            if (match) {
                // --- GENERACIÓN DE FIRMA JNI ---
                std::string finalSig;
                std::string rawType = currentType;
                std::replace(rawType.begin(), rawType.end(), '.', '/');

                // Si empieza con '[' es array, se queda igual. Si no, es objeto 'L...;'
                // Excepción: Primitivos (int, float, boolean) no llevan L ni ; pero sus nombres son cortos.
                // Esta lógica asume Objetos. Para primitivos, el tipo viene como "int", "boolean".

                if (rawType == "int") finalSig = "I";
                else if (rawType == "boolean") finalSig = "Z";
                else if (rawType == "float") finalSig = "F";
                else if (rawType == "double") finalSig = "D";
                else if (rawType == "long") finalSig = "J";
                else if (rawType.find("[") == 0) finalSig = rawType;
                else finalSig = "L" + rawType + ";";

                // Guardar en el Mapper
                Java::Mapper::fields[fieldKey] = currentName;
                Java::Mapper::fieldSigs[fieldKey] = finalSig;

                std::cout << "[MAPPER OK] " << fieldKey << " -> " << currentName << " (" << finalSig << ")" << std::endl;

                found = true;
            }

            Java::CEnv::env->DeleteLocalRef(typeObj);
            Java::CEnv::env->DeleteLocalRef(nameStr);
            Java::CEnv::env->DeleteLocalRef(fieldObj);

            if (found) break;
        }

        Java::CEnv::env->DeleteLocalRef(fieldsArray);
        Java::CEnv::env->DeleteLocalRef(fieldClass);
        Java::CEnv::env->DeleteLocalRef(classOfClass);
        Java::CEnv::env->DeleteLocalRef(classClass);
        Java::CEnv::env->DeleteLocalRef(cls);

        if (!found) {
            std::cout << "[MAPPER FAIL] No se encontro campo: " << fieldKey << std::endl;
        }
    }

    void Expert::ResolveMethodGeneric(std::string classKey, std::string methodKey, std::vector<std::string> nameCandidates, std::string returnTypeKey, int argsCount) {
        // 1. Validar Clase
        if (Java::Mapper::classes.find(classKey)
            == Java::Mapper::classes.end()) return;
        std::string className = Java::Mapper::classes[classKey];
        jclass cls = Utils::FindClass(className.c_str());
        if (!cls) { Java::CEnv::env->ExceptionClear(); return; }

        // 2. Obtener Métodos
        jclass classClass = Java::CEnv::env->GetObjectClass(cls);
        jmethodID getDeclaredMethods = Java::CEnv::env->GetMethodID(classClass, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
        jobjectArray methodsArray = (jobjectArray)Java::CEnv::env->CallObjectMethod(cls, getDeclaredMethods);
        if (!methodsArray) { Java::CEnv::env->DeleteLocalRef(cls); return; }

        jsize count = Java::CEnv::env->GetArrayLength(methodsArray);

        // Helpers
        jclass methodClass = Java::CEnv::env->FindClass("java/lang/reflect/Method");
        jmethodID getName = Java::CEnv::env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");
        jmethodID getReturnType = Java::CEnv::env->GetMethodID(methodClass, "getReturnType", "()Ljava/lang/Class;");
        jmethodID getParameterTypes = Java::CEnv::env->GetMethodID(methodClass, "getParameterTypes", "()[Ljava/lang/Class;");

        jclass classOfClass = Java::CEnv::env->FindClass("java/lang/Class");
        jmethodID getClassName = Java::CEnv::env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

        // --- NUEVA LÓGICA DE TIPOS ---
        std::string targetReturnType = "";

        // A) Si es primitivo, usamos el nombre directo
        if (returnTypeKey == "float" || returnTypeKey == "int" || returnTypeKey == "boolean" || returnTypeKey == "void" || returnTypeKey == "double") {
            targetReturnType = returnTypeKey;
        }
        // B) Si es una clase ofuscada, la buscamos en el mapa
        else if (!returnTypeKey.empty()
            && Java::Mapper::classes.find(returnTypeKey)
            != Java::Mapper::classes.end()) {
            targetReturnType = Java::Mapper::classes[returnTypeKey];
        }

        bool found = false;

        for (int i = 0; i < count; i++) {
            jobject methodObj = Java::CEnv::env->GetObjectArrayElement(methodsArray, i);

            // Check Argumentos
            jobjectArray params = (jobjectArray)Java::CEnv::env->CallObjectMethod(methodObj, getParameterTypes);
            jsize pCount = Java::CEnv::env->GetArrayLength(params);
            Java::CEnv::env->DeleteLocalRef(params);

            if (pCount != argsCount) {
                Java::CEnv::env->DeleteLocalRef(methodObj);
                continue;
            }

            // Obtener Datos
            jstring nameStr = (jstring)Java::CEnv::env->CallObjectMethod(methodObj, getName);
            const char* nameChars = Java::CEnv::env->GetStringUTFChars(nameStr, 0);
            std::string currentName(nameChars);
            Java::CEnv::env->ReleaseStringUTFChars(nameStr, nameChars);

            jobject returnTypeObj = Java::CEnv::env->CallObjectMethod(methodObj, getReturnType);
            jstring retTypeNameStr = (jstring)Java::CEnv::env->CallObjectMethod(returnTypeObj, getClassName);
            const char* retChars = Java::CEnv::env->GetStringUTFChars(retTypeNameStr, 0);
            std::string currentRetType(retChars);
            Java::CEnv::env->ReleaseStringUTFChars(retTypeNameStr, retChars);
            Java::CEnv::env->DeleteLocalRef(returnTypeObj);

            bool match = false;

            // 1. Nombre
            for (const auto& cand : nameCandidates) {
                if (currentName == cand) { match = true; break; }
            }

            // 2. Tipo (Smart Match)
            if (!match && !targetReturnType.empty()) {
                if (currentRetType == targetReturnType) {
                    // Evitamos confundir "float" con "java.lang.Float" (Wrapper)
                    match = true;
                }
            }

            if (match) {
                // --- CONSTRUCCIÓN DE FIRMA SOPORTANDO PRIMITIVOS ---
                std::string finalSigPart;
                std::string rawType = currentRetType;

                if (rawType == "void") finalSigPart = "V";
                else if (rawType == "int") finalSigPart = "I";
                else if (rawType == "boolean") finalSigPart = "Z";
                else if (rawType == "float") finalSigPart = "F";  // <--- AQUÍ ESTÁ EL FLOAT
                else if (rawType == "double") finalSigPart = "D";
                else if (rawType == "long") finalSigPart = "J";
                else if (rawType == "short") finalSigPart = "S";
                else if (rawType == "byte") finalSigPart = "B";
                else {
                    // Es un objeto
                    std::replace(rawType.begin(), rawType.end(), '.', '/');
                    if (rawType.find("[") == 0) finalSigPart = rawType;
                    else finalSigPart = "L" + rawType + ";";
                }

                // Asumimos args () porque argsCount logicamente es para getters simples
                // Si necesitaras args complejos, habría que construir esa parte también.
                std::string finalSig = "()" + finalSigPart;

                Java::Mapper::methods[methodKey] = currentName;
                Java::Mapper::methodSigs[methodKey] = finalSig;

                std::cout << "   [EXITO] " << methodKey << " -> " << currentName << " " << finalSig << std::endl;
                found = true;
            }

            Java::CEnv::env->DeleteLocalRef(nameStr);
            Java::CEnv::env->DeleteLocalRef(methodObj);

            if (found) break;
        }

        // Limpiezas...
        Java::CEnv::env->DeleteLocalRef(methodsArray);
        Java::CEnv::env->DeleteLocalRef(methodClass);
        Java::CEnv::env->DeleteLocalRef(classOfClass);
        Java::CEnv::env->DeleteLocalRef(classClass);
        Java::CEnv::env->DeleteLocalRef(cls);
    }
}