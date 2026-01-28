#include "helperExpert.h"

namespace Helper {
    void Expert::ResolveFieldGeneric(std::string classKey, std::string fieldKey, std::vector<std::string> nameCandidates, std::string expectedTypeKey) {
        // 1. Validar que la clase existe
        if (Java::CMapper::Get().classes.find(classKey)
            == Java::CMapper::Get().classes.end()) {
            LOG_ERROR(xorstr("[MAPPER ERROR] Clase no registrada: ") + classKey);
            return;
        }

        std::string className = Java::CMapper::Get().classes[classKey];
        jclass cls = Utils::FindClass(className.c_str());
        if (!cls) { Java::CEnv::Get().env->ExceptionClear(); return; }

        // 2. Obtener TODOS los campos de la clase
        jclass classClass = Java::CEnv::Get().env->GetObjectClass(cls);
        jmethodID getDeclaredFields = Java::CEnv::Get().env->GetMethodID(classClass, xorstr("getDeclaredFields"), xorstr("()[Ljava/lang/reflect/Field;"));
        jobjectArray fieldsArray = (jobjectArray)Java::CEnv::Get().env->CallObjectMethod(cls, getDeclaredFields);

        if (!fieldsArray) {
            Java::CEnv::Get().env->DeleteLocalRef(cls);
            return;
        }

        jsize count = Java::CEnv::Get().env->GetArrayLength(fieldsArray);

        // Métodos de Reflexión
        jclass fieldClass = Java::CEnv::Get().env->FindClass(xorstr("java/lang/reflect/Field"));
        jmethodID getName = Java::CEnv::Get().env->GetMethodID(fieldClass, xorstr("getName"), xorstr("()Ljava/lang/String;"));
        jmethodID getType = Java::CEnv::Get().env->GetMethodID(fieldClass, xorstr("getType"), xorstr("()Ljava/lang/Class;"));
        jclass classOfClass = Java::CEnv::Get().env->FindClass(xorstr("java/lang/Class"));
        jmethodID getClassName = Java::CEnv::Get().env->GetMethodID(classOfClass, xorstr("getName"), xorstr("()Ljava/lang/String;"));

        bool found = false;

        // Resolver el nombre de la clase del tipo esperado (si se dio uno)
        std::string targetType = "";
        if (!expectedTypeKey.empty()
            && Java::CMapper::Get().classes.find(expectedTypeKey)
            != Java::CMapper::Get().classes.end()) {
            targetType = Java::CMapper::Get().classes[expectedTypeKey];
        }

        for (int i = 0; i < count; i++) {
            jobject fieldObj = Java::CEnv::Get().env->GetObjectArrayElement(fieldsArray, i);

            // A) Obtener Nombre del Campo
            jstring nameStr = (jstring)Java::CEnv::Get().env->CallObjectMethod(fieldObj, getName);
            const char* nameChars = Java::CEnv::Get().env->GetStringUTFChars(nameStr, 0);
            std::string currentName(nameChars);
            Java::CEnv::Get().env->ReleaseStringUTFChars(nameStr, nameChars);

            // B) Obtener Tipo del Campo (Nombre de la clase)
            jobject typeObj = Java::CEnv::Get().env->CallObjectMethod(fieldObj, getType);
            jstring typeStr = (jstring)Java::CEnv::Get().env->CallObjectMethod(typeObj, getClassName);
            const char* typeChars = Java::CEnv::Get().env->GetStringUTFChars(typeStr, 0);
            std::string currentType(typeChars);
            Java::CEnv::Get().env->ReleaseStringUTFChars(typeStr, typeChars);

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
                    // Debug opcional: DEBUG_LOG("   [SMART] Campo '" + currentName + "' encontrado por TIPO.");
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

                if (rawType == xorstr("int")) finalSig = xorstr("I");
                else if (rawType == xorstr("boolean")) finalSig = xorstr("Z");
                else if (rawType == xorstr("float")) finalSig = xorstr("F");
                else if (rawType == xorstr("double")) finalSig = xorstr("D");
                else if (rawType == xorstr("long")) finalSig = xorstr("J");
                else if (rawType.find(xorstr("[")) == 0) finalSig = rawType;
                else finalSig = xorstr("L") + rawType + xorstr(";");

                // Guardar en el Mapper
                Java::CMapper::Get().fields[fieldKey] = currentName;
                Java::CMapper::Get().fieldSigs[fieldKey] = finalSig;

                LOG_INFO(xorstr("[MAPPER OK] ") + fieldKey + xorstr(" -> ") + currentName + xorstr(" (") + finalSig + ")");

                found = true;
            }

            Java::CEnv::Get().env->DeleteLocalRef(typeObj);
            Java::CEnv::Get().env->DeleteLocalRef(nameStr);
            Java::CEnv::Get().env->DeleteLocalRef(fieldObj);

            if (found) break;
        }

        Java::CEnv::Get().env->DeleteLocalRef(fieldsArray);
        Java::CEnv::Get().env->DeleteLocalRef(fieldClass);
        Java::CEnv::Get().env->DeleteLocalRef(classOfClass);
        Java::CEnv::Get().env->DeleteLocalRef(classClass);
        Java::CEnv::Get().env->DeleteLocalRef(cls);

        if (!found) {
            LOG_WARN(xorstr("[MAPPER FAIL] No se encontro campo: ") + fieldKey);
        }
    }

    void Expert::ResolveMethodGeneric(std::string classKey, std::string methodKey, std::vector<std::string> nameCandidates, std::string returnTypeKey, int argsCount) {
        // 1. Validar Clase
        if (Java::CMapper::Get().classes.find(classKey)
            == Java::CMapper::Get().classes.end()) return;
        std::string className = Java::CMapper::Get().classes[classKey];
        jclass cls = Utils::FindClass(className.c_str());
        if (!cls) { Java::CEnv::Get().env->ExceptionClear(); return; }

        // 2. Obtener Métodos
        jclass classClass = Java::CEnv::Get().env->GetObjectClass(cls);
        jmethodID getDeclaredMethods = Java::CEnv::Get().env->GetMethodID(classClass, xorstr("getDeclaredMethods"), xorstr("()[Ljava/lang/reflect/Method;"));
        jobjectArray methodsArray = (jobjectArray)Java::CEnv::Get().env->CallObjectMethod(cls, getDeclaredMethods);
        if (!methodsArray) { Java::CEnv::Get().env->DeleteLocalRef(cls); return; }

        jsize count = Java::CEnv::Get().env->GetArrayLength(methodsArray);

        // Helpers
        jclass methodClass = Java::CEnv::Get().env->FindClass(xorstr("java/lang/reflect/Method"));
        jmethodID getName = Java::CEnv::Get().env->GetMethodID(methodClass, xorstr("getName"), xorstr("()Ljava/lang/String;"));
        jmethodID getReturnType = Java::CEnv::Get().env->GetMethodID(methodClass, xorstr("getReturnType"), xorstr("()Ljava/lang/Class;"));
        jmethodID getParameterTypes = Java::CEnv::Get().env->GetMethodID(methodClass, xorstr("getParameterTypes"), xorstr("()[Ljava/lang/Class;"));

        jclass classOfClass = Java::CEnv::Get().env->FindClass(xorstr("java/lang/Class"));
        jmethodID getClassName = Java::CEnv::Get().env->GetMethodID(classOfClass, xorstr("getName"), xorstr("()Ljava/lang/String;"));

        // --- NUEVA LÓGICA DE TIPOS ---
        std::string targetReturnType = xorstr("");

        // A) Si es primitivo, usamos el nombre directo
        if (returnTypeKey == xorstr("float") || returnTypeKey == xorstr("int") || returnTypeKey == xorstr("boolean") || returnTypeKey == xorstr("void") || returnTypeKey == xorstr("double")) {
            targetReturnType = returnTypeKey;
        }
        // B) Si es una clase ofuscada, la buscamos en el mapa
        else if (!returnTypeKey.empty()
            && Java::CMapper::Get().classes.find(returnTypeKey)
            != Java::CMapper::Get().classes.end()) {
            targetReturnType = Java::CMapper::Get().classes[returnTypeKey];
        }

        bool found = false;

        for (int i = 0; i < count; i++) {
            jobject methodObj = Java::CEnv::Get().env->GetObjectArrayElement(methodsArray, i);

            // Check Argumentos
            jobjectArray params = (jobjectArray)Java::CEnv::Get().env->CallObjectMethod(methodObj, getParameterTypes);
            jsize pCount = Java::CEnv::Get().env->GetArrayLength(params);
            Java::CEnv::Get().env->DeleteLocalRef(params);

            if (pCount != argsCount) {
                Java::CEnv::Get().env->DeleteLocalRef(methodObj);
                continue;
            }

            // Obtener Datos
            jstring nameStr = (jstring)Java::CEnv::Get().env->CallObjectMethod(methodObj, getName);
            const char* nameChars = Java::CEnv::Get().env->GetStringUTFChars(nameStr, 0);
            std::string currentName(nameChars);
            Java::CEnv::Get().env->ReleaseStringUTFChars(nameStr, nameChars);

            jobject returnTypeObj = Java::CEnv::Get().env->CallObjectMethod(methodObj, getReturnType);
            jstring retTypeNameStr = (jstring)Java::CEnv::Get().env->CallObjectMethod(returnTypeObj, getClassName);
            const char* retChars = Java::CEnv::Get().env->GetStringUTFChars(retTypeNameStr, 0);
            std::string currentRetType(retChars);
            Java::CEnv::Get().env->ReleaseStringUTFChars(retTypeNameStr, retChars);
            Java::CEnv::Get().env->DeleteLocalRef(returnTypeObj);

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

                if (rawType == xorstr("void")) finalSigPart = xorstr("V");
                else if (rawType == xorstr("int")) finalSigPart = xorstr("I");
                else if (rawType == xorstr("boolean")) finalSigPart = xorstr("Z");
                else if (rawType == xorstr("float")) finalSigPart = xorstr("F");  // <--- AQUÍ ESTÁ EL FLOAT
                else if (rawType == xorstr("double")) finalSigPart = xorstr("D");
                else if (rawType == xorstr("long")) finalSigPart = xorstr("J");
                else if (rawType == xorstr("short")) finalSigPart = xorstr("S");
                else if (rawType == xorstr("byte")) finalSigPart = xorstr("B");
                else {
                    // Es un objeto
                    std::replace(rawType.begin(), rawType.end(), '.', '/');
                    if (rawType.find(xorstr("[")) == 0) finalSigPart = rawType;
                    else finalSigPart = xorstr("L") + rawType + xorstr(";");
                }

                // Asumimos args () porque argsCount logicamente es para getters simples
                // Si necesitaras args complejos, habría que construir esa parte también.
                std::string finalSig = xorstr("()") + finalSigPart;

                Java::CMapper::Get().methods[methodKey] = currentName;
                Java::CMapper::Get().methodSigs[methodKey] = finalSig;

                LOG_INFO(xorstr("   [EXITO] ") + methodKey + xorstr(" -> ") + currentName + xorstr(" ") + finalSig);
                found = true;
            }

            Java::CEnv::Get().env->DeleteLocalRef(nameStr);
            Java::CEnv::Get().env->DeleteLocalRef(methodObj);

            if (found) break;
        }

        // Limpiezas...
        Java::CEnv::Get().env->DeleteLocalRef(methodsArray);
        Java::CEnv::Get().env->DeleteLocalRef(methodClass);
        Java::CEnv::Get().env->DeleteLocalRef(classOfClass);
        Java::CEnv::Get().env->DeleteLocalRef(classClass);
        Java::CEnv::Get().env->DeleteLocalRef(cls);
    }
}