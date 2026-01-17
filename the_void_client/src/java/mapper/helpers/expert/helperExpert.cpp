#include "helperExpert.h"

namespace TheVoid {
	namespace Helper {
        void Expert::ResolveFieldGeneric(std::string classKey, std::string fieldKey, std::vector<std::string> nameCandidates, std::string expectedTypeKey) {
            // 1. Validar que la clase existe
            if (TheVoid::Java::Mapper::classes.find(classKey) 
                == TheVoid::Java::Mapper::classes.end()) {
                std::cout << "[MAPPER ERROR] Clase no registrada: " << classKey << std::endl;
                return;
            }

            std::string className = TheVoid::Java::Mapper::classes[classKey];
            jclass cls = Utils::FindClass(className.c_str());
            if (!cls) { TheVoid::Java::env->ExceptionClear(); return; }

            // 2. Obtener TODOS los campos de la clase
            jclass classClass = TheVoid::Java::env->GetObjectClass(cls);
            jmethodID getDeclaredFields = TheVoid::Java::env->GetMethodID(classClass, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
            jobjectArray fieldsArray = (jobjectArray)TheVoid::Java::env->CallObjectMethod(cls, getDeclaredFields);

            if (!fieldsArray) {
                TheVoid::Java::env->DeleteLocalRef(cls);
                return;
            }

            jsize count = TheVoid::Java::env->GetArrayLength(fieldsArray);

            // Métodos de Reflexión
            jclass fieldClass = TheVoid::Java::env->FindClass("java/lang/reflect/Field");
            jmethodID getName = TheVoid::Java::env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
            jmethodID getType = TheVoid::Java::env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
            jclass classOfClass = TheVoid::Java::env->FindClass("java/lang/Class");
            jmethodID getClassName = TheVoid::Java::env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

            bool found = false;

            // Resolver el nombre de la clase del tipo esperado (si se dio uno)
            std::string targetType = "";
            if (!expectedTypeKey.empty() 
                && TheVoid::Java::Mapper::classes.find(expectedTypeKey) 
                != TheVoid::Java::Mapper::classes.end()) {
                targetType = TheVoid::Java::Mapper::classes[expectedTypeKey];
            }

            for (int i = 0; i < count; i++) {
                jobject fieldObj = TheVoid::Java::env->GetObjectArrayElement(fieldsArray, i);

                // A) Obtener Nombre del Campo
                jstring nameStr = (jstring)TheVoid::Java::env->CallObjectMethod(fieldObj, getName);
                const char* nameChars = TheVoid::Java::env->GetStringUTFChars(nameStr, 0);
                std::string currentName(nameChars);
                TheVoid::Java::env->ReleaseStringUTFChars(nameStr, nameChars);

                // B) Obtener Tipo del Campo (Nombre de la clase)
                jobject typeObj = TheVoid::Java::env->CallObjectMethod(fieldObj, getType);
                jstring typeStr = (jstring)TheVoid::Java::env->CallObjectMethod(typeObj, getClassName);
                const char* typeChars = TheVoid::Java::env->GetStringUTFChars(typeStr, 0);
                std::string currentType(typeChars);
                TheVoid::Java::env->ReleaseStringUTFChars(typeStr, typeChars);

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
                    TheVoid::Java::Mapper::fields[fieldKey] = currentName;
                    TheVoid::Java::Mapper::fieldSigs[fieldKey] = finalSig;

                    std::cout << "[MAPPER OK] " << fieldKey << " -> " << currentName << " (" << finalSig << ")" << std::endl;

                    found = true;
                }

                TheVoid::Java::env->DeleteLocalRef(typeObj);
                TheVoid::Java::env->DeleteLocalRef(nameStr);
                TheVoid::Java::env->DeleteLocalRef(fieldObj);

                if (found) break;
            }

            TheVoid::Java::env->DeleteLocalRef(fieldsArray);
            TheVoid::Java::env->DeleteLocalRef(fieldClass);
            TheVoid::Java::env->DeleteLocalRef(classOfClass);
            TheVoid::Java::env->DeleteLocalRef(classClass);
            TheVoid::Java::env->DeleteLocalRef(cls);

            if (!found) {
                std::cout << "[MAPPER FAIL] No se encontro campo: " << fieldKey << std::endl;
            }
        }

        void Expert::ResolveMethodGeneric(std::string classKey, std::string methodKey, std::vector<std::string> nameCandidates, std::string returnTypeKey, int argsCount) {
            // 1. Validar Clase
            if (TheVoid::Java::Mapper::classes.find(classKey) 
                == TheVoid::Java::Mapper::classes.end()) return;
            std::string className = TheVoid::Java::Mapper::classes[classKey];
            jclass cls = Utils::FindClass(className.c_str());
            if (!cls) { TheVoid::Java::env->ExceptionClear(); return; }

            // 2. Obtener Métodos
            jclass classClass = TheVoid::Java::env->GetObjectClass(cls);
            jmethodID getDeclaredMethods = TheVoid::Java::env->GetMethodID(classClass, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
            jobjectArray methodsArray = (jobjectArray)TheVoid::Java::env->CallObjectMethod(cls, getDeclaredMethods);
            if (!methodsArray) { TheVoid::Java::env->DeleteLocalRef(cls); return; }

            jsize count = TheVoid::Java::env->GetArrayLength(methodsArray);

            // Helpers
            jclass methodClass = TheVoid::Java::env->FindClass("java/lang/reflect/Method");
            jmethodID getName = TheVoid::Java::env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");
            jmethodID getReturnType = TheVoid::Java::env->GetMethodID(methodClass, "getReturnType", "()Ljava/lang/Class;");
            jmethodID getParameterTypes = TheVoid::Java::env->GetMethodID(methodClass, "getParameterTypes", "()[Ljava/lang/Class;");

            jclass classOfClass = TheVoid::Java::env->FindClass("java/lang/Class");
            jmethodID getClassName = TheVoid::Java::env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

            // --- NUEVA LÓGICA DE TIPOS ---
            std::string targetReturnType = "";

            // A) Si es primitivo, usamos el nombre directo
            if (returnTypeKey == "float" || returnTypeKey == "int" || returnTypeKey == "boolean" || returnTypeKey == "void" || returnTypeKey == "double") {
                targetReturnType = returnTypeKey;
            }
            // B) Si es una clase ofuscada, la buscamos en el mapa
            else if (!returnTypeKey.empty() 
                && TheVoid::Java::Mapper::classes.find(returnTypeKey) 
                != TheVoid::Java::Mapper::classes.end()) {
                targetReturnType = TheVoid::Java::Mapper::classes[returnTypeKey];
            }

            bool found = false;

            for (int i = 0; i < count; i++) {
                jobject methodObj = TheVoid::Java::env->GetObjectArrayElement(methodsArray, i);

                // Check Argumentos
                jobjectArray params = (jobjectArray)TheVoid::Java::env->CallObjectMethod(methodObj, getParameterTypes);
                jsize pCount = TheVoid::Java::env->GetArrayLength(params);
                TheVoid::Java::env->DeleteLocalRef(params);

                if (pCount != argsCount) {
                    TheVoid::Java::env->DeleteLocalRef(methodObj);
                    continue;
                }

                // Obtener Datos
                jstring nameStr = (jstring)TheVoid::Java::env->CallObjectMethod(methodObj, getName);
                const char* nameChars = TheVoid::Java::env->GetStringUTFChars(nameStr, 0);
                std::string currentName(nameChars);
                TheVoid::Java::env->ReleaseStringUTFChars(nameStr, nameChars);

                jobject returnTypeObj = TheVoid::Java::env->CallObjectMethod(methodObj, getReturnType);
                jstring retTypeNameStr = (jstring)TheVoid::Java::env->CallObjectMethod(returnTypeObj, getClassName);
                const char* retChars = TheVoid::Java::env->GetStringUTFChars(retTypeNameStr, 0);
                std::string currentRetType(retChars);
                TheVoid::Java::env->ReleaseStringUTFChars(retTypeNameStr, retChars);
                TheVoid::Java::env->DeleteLocalRef(returnTypeObj);

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

                    TheVoid::Java::Mapper::methods[methodKey] = currentName;
                    TheVoid::Java::Mapper::methodSigs[methodKey] = finalSig;

                    std::cout << "   [EXITO] " << methodKey << " -> " << currentName << " " << finalSig << std::endl;
                    found = true;
                }

                TheVoid::Java::env->DeleteLocalRef(nameStr);
                TheVoid::Java::env->DeleteLocalRef(methodObj);

                if (found) break;
            }

            // Limpiezas...
            TheVoid::Java::env->DeleteLocalRef(methodsArray);
            TheVoid::Java::env->DeleteLocalRef(methodClass);
            TheVoid::Java::env->DeleteLocalRef(classOfClass);
            TheVoid::Java::env->DeleteLocalRef(classClass);
            TheVoid::Java::env->DeleteLocalRef(cls);
        }
	}
}