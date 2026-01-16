#include "Mapper.h"
 
namespace TheVoid {
    namespace Java {
        std::unordered_map<std::string, std::string> Mapper::classes;
        std::unordered_map<std::string, std::string> Mapper::methods;
        std::unordered_map<std::string, std::string> Mapper::fields;
        std::unordered_map<std::string, std::string> Mapper::fieldSigs;
        std::unordered_map<std::string, std::string> Mapper::methodSigs;
        std::unordered_map<std::string, jclass> Mapper::classCache;

        // ==========================================================
                // LÓGICA DE APRENDIZAJE DINÁMICO (CERO LAG)
                // ==========================================================
        bool Mapper::LearnAndCheck(jobject obj, std::string cacheKey, std::vector<std::string> searchPatterns) {
            if (!obj) return false;

            // 1. CACHE HIT (Ruta Rápida)
            // Si ya aprendimos qué es esto, usamos IsInstanceOf (Nativo y veloz)
            if (classCache.find(cacheKey) != classCache.end()) {
                return Utils::IsInstanceOf(obj, classCache[cacheKey]);
            }

            // 2. CACHE MISS (Ruta de Aprendizaje - Solo ocurre 1 vez por tipo de pantalla)
            // Obtenemos el nombre real usando Reflexión
            std::string realName = Utils::GetClassName(obj);
            if (realName.empty()) return false;

            // Buscamos si el nombre coincide con algún patrón
            for (const auto& pattern : searchPatterns) {
                // .find() para Forge/Lunar (nombres largos), == para Vanilla (nombres cortos)
                if (realName.find(pattern) != std::string::npos || realName == pattern) {

                    std::cout << "[MAPPER] Aprendido: " << cacheKey << " (" << pattern << ") detectado como -> " << realName << std::endl;

                    // IMPORTANTE: Convertir a GlobalRef para que no se borre
                    jclass localCls = env->GetObjectClass(obj);
                    jclass globalCls = (jclass)env->NewGlobalRef(localCls);

                    // Guardar en Caché
                    classCache[cacheKey] = globalCls;

                    env->DeleteLocalRef(localCls);
                    return true;
                }
            }
            return false;
        }

        void Mapper::ResolveClass(std::string internalName, std::vector<std::string> candidates) {
            std::cout << "[DEBUG] Buscando Clase: " << internalName << "..." << std::endl;
            for (const auto& name : candidates) {
                jclass cls = Utils::FindClass(name.c_str());
                if (cls) {
                    classes[internalName] = name;
                    env->DeleteLocalRef(cls);
                    std::cout << "   [OK] Encontrada como: " << name << std::endl;
                    return;
                }
                env->ExceptionClear();
            }
            std::cout << "   [FALLO FATAL] No se encontro la clase: " << internalName << std::endl;
        }

        void Mapper::ResolveField(std::string classKey, std::string fieldKey, std::vector<std::string> candidates, std::vector<std::string> sigCandidates) {
            // SEGURIDAD 1: ¿Resolvimos la clase antes?
            if (classes.find(classKey) == classes.end()) {
                std::cout << "[SKIP] Saltando campo '" << fieldKey << "' porque la clase '" << classKey << "' no existe." << std::endl;
                return;
            }

            std::string realClassName = classes[classKey];
            if (realClassName.empty()) return; // SEGURIDAD 2: Cadena vacía

            jclass cls = Utils::FindClass(realClassName.c_str());
            if (!cls) { env->ExceptionClear(); return; }

            for (const auto& name : candidates) {
                for (const auto& sig : sigCandidates) {
                    jfieldID fid = env->GetFieldID(cls, name.c_str(), sig.c_str());
                    if (fid) {
                        fields[fieldKey] = name;
                        fieldSigs[fieldKey] = sig;
                        env->DeleteLocalRef(cls);
                        // Log silencioso para no spamear, descomenta si quieres ver todo
                        std::cout << "   [FIELD OK] " << fieldKey << " -> " << name << std::endl;
                        return;
                    }
                    env->ExceptionClear();
                }
            }
            env->DeleteLocalRef(cls);
            std::cout << "   [FIELD FALLO] No se encontro campo: " << fieldKey << std::endl;
        }

        void Mapper::ResolveMethod(std::string classKey, std::string methodKey, std::vector<std::string> candidates, std::string signature) {
            // SEGURIDAD: Mismo chequeo que en fields
            if (classes.find(classKey) == classes.end()) return;

            jclass cls = Utils::FindClass(classes[classKey].c_str());
            if (!cls) { env->ExceptionClear(); return; }

            for (const auto& name : candidates) {
                jmethodID mid = env->GetMethodID(cls, name.c_str(), signature.c_str());
                if (mid) {
                    methods[methodKey] = name;
                    env->DeleteLocalRef(cls);
                    return;
                }
                env->ExceptionClear();
            }
            env->DeleteLocalRef(cls);
            std::cout << "   [METHOD FALLO] No se encontro metodo: " << methodKey << std::endl;
        }

        void Mapper::ResolveFieldByAnalysis(std::string classKey, std::string fieldKey, std::vector<std::string> candidates) {
            // 1. Obtener la clase base (MovingObjectPosition)
            if (classes.find(classKey) == classes.end()) return;
            jclass cls = Utils::FindClass(classes[classKey].c_str());
            if (!cls) { env->ExceptionClear(); return; }

            std::cout << "[MAPPER ANALYZER] Buscando campo '" << fieldKey << "' en " << classes[classKey] << "..." << std::endl;

            // 2. Obtener TODOS los campos (Reflexión)
            jclass classClass = env->GetObjectClass(cls);
            jmethodID getFields = env->GetMethodID(classClass, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
            jobjectArray fieldsArray = (jobjectArray)env->CallObjectMethod(cls, getFields);

            if (!fieldsArray) {
                env->ExceptionClear();
                env->DeleteLocalRef(cls);
                return;
            }

            // Preparar métodos de reflexión
            jclass fieldClass = env->FindClass("java/lang/reflect/Field");
            jmethodID getName = env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
            jmethodID getType = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
            jmethodID getTypeName = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");

            jsize count = env->GetArrayLength(fieldsArray);
            bool found = false;

            for (int i = 0; i < count; i++) {
                jobject fieldObj = env->GetObjectArrayElement(fieldsArray, i);

                // Obtener Nombre del campo actual
                jstring nameStr = (jstring)env->CallObjectMethod(fieldObj, getName);
                const char* nameChars = env->GetStringUTFChars(nameStr, 0);
                std::string currentName(nameChars);
                env->ReleaseStringUTFChars(nameStr, nameChars);

                // --- ¿Coincide con alguno de nuestros candidatos? ---
                for (const auto& cand : candidates) {
                    if (currentName == cand) {
                        // ¡EUREKA! Encontramos el nombre. Ahora extraemos su FIRMA REAL.

                        jobject typeObj = env->CallObjectMethod(fieldObj, getType);
                        jstring typeStr = (jstring)env->CallObjectMethod(typeObj, getTypeName);
                        const char* typeChars = env->GetStringUTFChars(typeStr, 0);
                        std::string typeSignature(typeChars);
                        env->ReleaseStringUTFChars(typeStr, typeChars);

                        // --- CONVERSIÓN VITAL: Formato Java a Formato JNI ---
                        // Tu inspector mostró: "net.minecraft.util.MovingObjectPosition$MovingObjectType"
                        // JNI necesita: "Lnet/minecraft/util/MovingObjectPosition$MovingObjectType;"

                        // 1. Reemplazar puntos por barras
                        std::replace(typeSignature.begin(), typeSignature.end(), '.', '/');

                        // 2. Agregar 'L' y ';' (Si no es un array)
                        if (typeSignature.find("[") == std::string::npos) {
                            // Ignoramos primitivos (int, boolean), pero este campo es un Objeto
                            typeSignature = "L" + typeSignature + ";";
                        }

                        // Guardamos en el Mapper
                        fields[fieldKey] = currentName;
                        fieldSigs[fieldKey] = typeSignature; // <--- Firma perfecta

                        std::cout << "   [OK] Campo Detectado: " << currentName << " | Sig: " << typeSignature << std::endl;

                        env->DeleteLocalRef(typeObj);
                        found = true;
                        break;
                    }
                }

                env->DeleteLocalRef(nameStr);
                env->DeleteLocalRef(fieldObj);
                if (found) break;
            }

            // Limpieza
            env->DeleteLocalRef(fieldClass);
            env->DeleteLocalRef(classClass);
            env->DeleteLocalRef(cls);
        }

        void Mapper::ResolveFieldAndAutoDetectSignature(std::string classKey, std::string fieldKey, std::vector<std::string> candidates) {
            if (classes.find(classKey) == classes.end()) {
                std::cout << "[MAPPER ERROR] La clase " << classKey << " no esta resuelta." << std::endl;
                return;
            }

            jclass cls = Utils::FindClass(classes[classKey].c_str());
            if (!cls) { env->ExceptionClear(); return; }

            // Preparamos los métodos de reflexión necesarios
            jclass classClass = env->GetObjectClass(cls);
            jmethodID getDeclaredField = env->GetMethodID(classClass, "getDeclaredField", "(Ljava/lang/String;)Ljava/lang/reflect/Field;");

            jclass fieldClass = env->FindClass("java/lang/reflect/Field");
            jmethodID getType = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");

            // Método getName de la clase Class
            jclass classOfClass = env->FindClass("java/lang/Class");
            jmethodID getName = env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

            bool success = false;

            for (const auto& cand : candidates) {
                // 1. Intentamos obtener el campo por nombre DIRECTAMENTE
                // Esto lanzará excepción si no existe, así que la limpiamos.
                jstring nameStr = env->NewStringUTF(cand.c_str());
                jobject fieldObj = env->CallObjectMethod(cls, getDeclaredField, nameStr);
                env->DeleteLocalRef(nameStr);

                if (env->ExceptionCheck()) {
                    env->ExceptionClear(); // No existe este nombre, probamos el siguiente
                    continue;
                }

                if (fieldObj) {
                    // 2. ¡ENCONTRADO! Ahora obtenemos su tipo (La Clase del campo)
                    jobject typeClassObj = env->CallObjectMethod(fieldObj, getType);

                    // 3. Obtenemos el nombre legible del tipo (ej: "net.minecraft.util.MovingObjectPosition$MovingObjectType")
                    jstring typeNameStr = (jstring)env->CallObjectMethod(typeClassObj, getName);
                    const char* typeNameChars = env->GetStringUTFChars(typeNameStr, 0);
                    std::string rawTypeName(typeNameChars);
                    env->ReleaseStringUTFChars(typeNameStr, typeNameChars);

                    std::cout << "[MAPPER] Campo '" << cand << "' encontrado. Tipo raw: " << rawTypeName << std::endl;

                    // 4. CONVERSIÓN A FIRMA JNI (Estricta)
                    std::string finalSig;

                    // Reemplazar puntos por barras
                    std::replace(rawTypeName.begin(), rawTypeName.end(), '.', '/');

                    // Formatear según si es array o objeto
                    if (rawTypeName.find("[") == 0) {
                        // Ya es un array (ej: "[Lnet/minecraft..."), se deja igual pero con barras
                        finalSig = rawTypeName;
                    }
                    else {
                        // Es un objeto (ej: "net/minecraft..."), agregamos L y ;
                        // IMPORTANTE: JNI es sensible a espacios, aseguramos limpieza.
                        finalSig = "L" + rawTypeName + ";";
                    }

                    // 5. VALIDACIÓN FINAL (La prueba de fuego)
                    // Intentamos obtener el FieldID AHORA MISMO con la firma generada.
                    // Si esto falla aquí, sabemos que la firma generada está mal antes de jugar.
                    jfieldID testFID = env->GetFieldID(cls, cand.c_str(), finalSig.c_str());

                    if (testFID) {
                        fields[fieldKey] = cand;
                        fieldSigs[fieldKey] = finalSig;
                        std::cout << "   [EXITO] Firma Generada y Validada: " << finalSig << std::endl;
                        success = true;
                    }
                    else {
                        std::cout << "   [ERROR] El campo existe pero la firma generada fallo: " << finalSig << std::endl;
                        env->ExceptionClear();
                    }

                    env->DeleteLocalRef(typeClassObj);
                    env->DeleteLocalRef(fieldObj);
                    env->DeleteLocalRef(typeNameStr); // Limpieza extra
                }

                if (success) break;
            }

            env->DeleteLocalRef(classClass);
            env->DeleteLocalRef(fieldClass);
            env->DeleteLocalRef(classOfClass);
            env->DeleteLocalRef(cls);

            if (!success) {
                std::cout << "[MAPPER FATAL] No se pudo resolver " << fieldKey << " con ningun candidato." << std::endl;
            }
        }

        // En Mapper.cpp

        void Mapper::ResolveFieldByIteration(std::string classKey, std::string fieldKey, std::vector<std::string> candidates) {
            if (classes.find(classKey) == classes.end()) 
                return;

            FindRealMOPClass();

            std::string className = classes[classKey];
            jclass cls = Utils::FindClass(className.c_str());
            if (!cls) { env->ExceptionClear(); return; }

            std::cout << "[MAPPER SCANNER] Analizando 9 campos de '" << className << "' para '" << fieldKey << "'..." << std::endl;

            jclass classClass = env->GetObjectClass(cls);
            jmethodID getDeclaredFields = env->GetMethodID(classClass, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
            jobjectArray fieldsArray = (jobjectArray)env->CallObjectMethod(cls, getDeclaredFields);

            if (!fieldsArray) { env->DeleteLocalRef(cls); return; }

            jsize count = env->GetArrayLength(fieldsArray);

            jclass fieldClass = env->FindClass("java/lang/reflect/Field");
            jmethodID getName = env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
            jmethodID getType = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");

            jclass classOfClass = env->FindClass("java/lang/Class");
            jmethodID getClassName = env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

            bool found = false;

            for (int i = 0; i < count; i++) {
                jobject fieldObj = env->GetObjectArrayElement(fieldsArray, i);

                // Obtener Nombre (ej: "c")
                jstring nameStr = (jstring)env->CallObjectMethod(fieldObj, getName);
                const char* nameChars = env->GetStringUTFChars(nameStr, 0);
                std::string currentName(nameChars);
                env->ReleaseStringUTFChars(nameStr, nameChars);

                // Obtener Tipo (ej: "auu$a")
                jobject typeObj = env->CallObjectMethod(fieldObj, getType);
                jstring typeStr = (jstring)env->CallObjectMethod(typeObj, getClassName);
                const char* typeChars = env->GetStringUTFChars(typeStr, 0);
                std::string currentType(typeChars);
                env->ReleaseStringUTFChars(typeStr, typeChars);

                // DEBUG: VER LA LISTA REAL
                // Esto te mostrará en consola: " -> Visto: c | Tipo: auu$a"
                std::cout << "   -> Visto: " << currentName << " | Tipo: " << currentType << std::endl;

                // --- LÓGICA DE COINCIDENCIA ---
                bool matchName = false;
                bool matchType = false;

                // 1. ¿Coincide el Nombre?
                for (const auto& cand : candidates) {
                    if (currentName == cand) { matchName = true; break; }
                }

                // 2. ¿Coincide el TIPO? (La Clave Maestra)
                // Buscamos si el tipo es el Enum (auu$a o MovingObjectType)
                if (currentType.find("MovingObjectType") != std::string::npos ||
                    currentType.find("auu$a") != std::string::npos) {
                    matchType = true;
                }

                if (matchName || matchType) {
                    // ¡ENCONTRADO! (Ya sea por nombre o por tipo)

                    // Construir Firma
                    std::string finalSig;
                    std::replace(currentType.begin(), currentType.end(), '.', '/');
                    if (currentType.find("[") == 0) finalSig = currentType;
                    else finalSig = "L" + currentType + ";";

                    // Guardar
                    fields[fieldKey] = currentName;
                    fieldSigs[fieldKey] = finalSig;

                    std::cout << "   [EXITO] MATCH POR " << (matchName ? "NOMBRE" : "TIPO") << "! -> " << currentName << " | Sig: " << finalSig << std::endl;

                    env->DeleteLocalRef(typeObj);
                    found = true;

                    env->DeleteLocalRef(nameStr);
                    env->DeleteLocalRef(fieldObj);
                    break;
                }

                env->DeleteLocalRef(typeObj);
                env->DeleteLocalRef(nameStr);
                env->DeleteLocalRef(fieldObj);
            }

            env->DeleteLocalRef(fieldsArray);
            env->DeleteLocalRef(fieldClass);
            env->DeleteLocalRef(classOfClass);
            env->DeleteLocalRef(classClass);
            env->DeleteLocalRef(cls);
        }

        void Mapper::ResolveMethodByAnalysis(std::string classKey, std::string methodKey, std::vector<std::string> candidates) {
            if (classes.find(classKey) == classes.end()) return;

            jclass cls = Utils::FindClass(classes[classKey].c_str());
            if (!cls) { env->ExceptionClear(); return; }

            std::cout << "[MAPPER METHOD] Analizando metodos de " << classes[classKey] << " para encontrar '" << methodKey << "'..." << std::endl;

            // Preparamos Reflexión
            jclass classClass = env->GetObjectClass(cls);
            // getDeclaredMethod(String name, Class... parameterTypes)
            jmethodID getDeclaredMethod = env->GetMethodID(classClass, "getDeclaredMethod", "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");

            jclass methodClass = env->FindClass("java/lang/reflect/Method");
            jmethodID getReturnType = env->GetMethodID(methodClass, "getReturnType", "()Ljava/lang/Class;");

            jclass classOfClass = env->FindClass("java/lang/Class");
            jmethodID getName = env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

            bool found = false;

            for (const auto& cand : candidates) {
                // 1. Intentamos obtener el método por nombre (Sin argumentos = null en el segundo param)
                jstring nameStr = env->NewStringUTF(cand.c_str());
                jobject methodObj = env->CallObjectMethod(cls, getDeclaredMethod, nameStr, nullptr);
                env->DeleteLocalRef(nameStr);

                if (env->ExceptionCheck()) {
                    env->ExceptionClear(); // No existe este nombre
                    continue;
                }

                if (methodObj) {
                    // ¡ENCONTRADO!
                    // 2. ¿Qué devuelve?
                    jobject returnTypeClass = env->CallObjectMethod(methodObj, getReturnType);
                    jstring returnTypeNameStr = (jstring)env->CallObjectMethod(returnTypeClass, getName);

                    const char* typeChars = env->GetStringUTFChars(returnTypeNameStr, 0);
                    std::string rawReturnType(typeChars);
                    env->ReleaseStringUTFChars(returnTypeNameStr, typeChars);

                    std::cout << "   [MATCH] Metodo '" << cand << "' encontrado. Retorna: " << rawReturnType << std::endl;

                    std::string returnSig;
                    std::replace(rawReturnType.begin(), rawReturnType.end(), '.', '/');

                    if (rawReturnType.find("[") == 0) {
                        returnSig = rawReturnType; // Ya es array
                    }
                    else {
                        // Es objeto
                        returnSig = "L" + rawReturnType + ";";
                    }

                    std::string finalSig = "()" + returnSig;

                    // Guardar
                    methods[methodKey] = cand;
                    jmethodID realMid = env->GetMethodID(cls, cand.c_str(), finalSig.c_str());
                    if (realMid) {
                        std::cout << "   [EXITO] Metodo Validado: " << cand << " | Sig: " << finalSig << std::endl;
                    }

                    env->DeleteLocalRef(returnTypeClass);
                    env->DeleteLocalRef(methodObj);
                    env->DeleteLocalRef(returnTypeNameStr); // Limpieza extra
                    found = true;
                    break;
                }
            }

            env->DeleteLocalRef(classOfClass);
            env->DeleteLocalRef(methodClass);
            env->DeleteLocalRef(cls);

            if (!found) {
                std::cout << "[MAPPER FAIL] No se encontro metodo " << methodKey << std::endl;
            }
        }

        void Mapper::FindRealMOPClass() {
            // 1. Obtener clase Minecraft (sabemos que es 'ave')
            std::string mcName = classes[xorstr("Minecraft")]; // Debería ser "ave"
            if (mcName.empty()) mcName = "ave";

            std::cout << "[MAPPER DETECTIVE] Entrando a Minecraft (" << mcName << ") para encontrar la verdadera clase MOP..." << std::endl;

            jclass mcClass = Utils::FindClass(mcName.c_str());
            if (!mcClass) {
                std::cout << "[MAPPER FATAL] No encuentro la clase Minecraft." << std::endl;
                return;
            }

            // 2. Buscamos el campo 's' (Es el nombre Vanilla 1.8.9 para objectMouseOver)
            // También probamos 'objectMouseOver' por si acaso.
            std::vector<std::string> candidates = { "s", "objectMouseOver", "field_71476_x" };

            jclass classClass = env->GetObjectClass(mcClass);
            jmethodID getDeclaredField = env->GetMethodID(classClass, "getDeclaredField", "(Ljava/lang/String;)Ljava/lang/reflect/Field;");
            jclass fieldClass = env->FindClass("java/lang/reflect/Field");
            jmethodID getType = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
            jclass classOfClass = env->FindClass("java/lang/Class");
            jmethodID getName = env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

            bool found = false;

            for (const auto& fieldName : candidates) {
                jstring nameStr = env->NewStringUTF(fieldName.c_str());

                // Intentamos obtener el campo
                jobject fieldObj = env->CallObjectMethod(mcClass, getDeclaredField, nameStr);
                env->DeleteLocalRef(nameStr);

                if (env->ExceptionCheck()) {
                    env->ExceptionClear(); // No existe este nombre
                    continue;
                }

                if (fieldObj) {
                    // ¡ENCONTRADO EL CAMPO!
                    // Ahora, ¿DE QUÉ TIPO ES? (Ese tipo será nuestra clase MovingObjectPosition)
                    jobject typeObj = env->CallObjectMethod(fieldObj, getType);
                    jstring typeNameStr = (jstring)env->CallObjectMethod(typeObj, getName);
                    const char* typeNameChars = env->GetStringUTFChars(typeNameStr, 0);
                    std::string realMOPClass(typeNameChars);
                    env->ReleaseStringUTFChars(typeNameStr, typeNameChars);

                    std::cout << "[EUREKA] El campo '" << fieldName << "' es de tipo: " << realMOPClass << std::endl;

                    // 3. ACTUALIZAMOS EL MAPPER CON LA VERDAD
                    // Sobreescribimos la entrada incorrecta (auu) con la real
                    classes[xorstr("MovingObjectPosition")] = realMOPClass;

                    // También necesitamos la clase del Enum. 
                    // Por convención es: RealClass$MovingObjectType o RealClass$a
                    // La guardamos como base, luego el scanner de campos lo confirmará.
                    classes[xorstr("MovingObjectType")] = realMOPClass + "$MovingObjectType"; // Fallback inicial

                    found = true;
                    env->DeleteLocalRef(typeObj);
                    env->DeleteLocalRef(fieldObj);
                    break;
                }
            }

            env->DeleteLocalRef(classClass);
            env->DeleteLocalRef(fieldClass);
            env->DeleteLocalRef(classOfClass);
            env->DeleteLocalRef(mcClass);

            if (!found) {
                std::cout << "[MAPPER FATAL] No pude encontrar el campo 's' en Minecraft. No puedo deducir MOP." << std::endl;
            }
        }

        void Mapper::ResolveMethodByReturnType(std::string classKey, std::string methodKey, std::string returnTypeClassKey) {
            if (classes.find(classKey) == classes.end() || classes.find(returnTypeClassKey) == classes.end()) {
                std::cout << "[MAPPER ERROR] Clases no definidas para resolver " << methodKey << std::endl;
                return;
            }

            std::string className = classes[classKey];
            std::string returnTypeName = classes[returnTypeClassKey]; // Nombre de la clase ItemStack (ej: zx)

            jclass cls = Utils::FindClass(className.c_str());
            if (!cls) { env->ExceptionClear(); return; }

            std::cout << "[MAPPER LOGIC] Buscando metodo en " << className << " que retorne " << returnTypeName << "..." << std::endl;

            // Obtener todos los métodos
            jclass classClass = env->GetObjectClass(cls);
            jmethodID getDeclaredMethods = env->GetMethodID(classClass, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
            jobjectArray methodsArray = (jobjectArray)env->CallObjectMethod(cls, getDeclaredMethods);

            if (!methodsArray) { env->DeleteLocalRef(cls); return; }

            jsize count = env->GetArrayLength(methodsArray);

            // Reflection helpers
            jclass methodClass = env->FindClass("java/lang/reflect/Method");
            jmethodID getName = env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");
            jmethodID getReturnType = env->GetMethodID(methodClass, "getReturnType", "()Ljava/lang/Class;");
            jmethodID getParameterTypes = env->GetMethodID(methodClass, "getParameterTypes", "()[Ljava/lang/Class;");

            jclass classOfClass = env->FindClass("java/lang/Class");
            jmethodID getClassName = env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

            bool found = false;

            for (int i = 0; i < count; i++) {
                jobject methodObj = env->GetObjectArrayElement(methodsArray, i);

                // 1. Checar Cantidad de Parámetros (Debe ser 0)
                jobjectArray params = (jobjectArray)env->CallObjectMethod(methodObj, getParameterTypes);
                jsize paramCount = env->GetArrayLength(params);
                env->DeleteLocalRef(params);

                if (paramCount == 0) {
                    // 2. Checar Tipo de Retorno
                    jobject returnTypeObj = env->CallObjectMethod(methodObj, getReturnType);
                    jstring returnTypeNameStr = (jstring)env->CallObjectMethod(returnTypeObj, getClassName);
                    const char* retChars = env->GetStringUTFChars(returnTypeNameStr, 0);
                    std::string currentRetType(retChars);
                    env->ReleaseStringUTFChars(returnTypeNameStr, retChars);
                    env->DeleteLocalRef(returnTypeObj);

                    // ¿El retorno coincide con ItemStack?
                    if (currentRetType == returnTypeName) {
                        // ¡LO ENCONTRAMOS!
                        jstring nameStr = (jstring)env->CallObjectMethod(methodObj, getName);
                        const char* nameChars = env->GetStringUTFChars(nameStr, 0);
                        std::string methodName(nameChars);
                        env->ReleaseStringUTFChars(nameStr, nameChars);

                        std::cout << "   [EUREKA] Metodo encontrado por logica: " << methodName << " -> Retorna: " << currentRetType << std::endl;

                        // Construir Firma
                        std::string finalSig;
                        std::string retSig = currentRetType;
                        std::replace(retSig.begin(), retSig.end(), '.', '/');

                        if (retSig.find("[") == 0) finalSig = "()" + retSig;
                        else finalSig = "()L" + retSig + ";";

                        // Guardar
                        methods[methodKey] = methodName;
                        methodSigs[methodKey] = finalSig;

                        found = true;
                    }
                }

                env->DeleteLocalRef(methodObj);
                if (found) break;
            }

            env->DeleteLocalRef(methodsArray);
            env->DeleteLocalRef(methodClass);
            env->DeleteLocalRef(classOfClass);
            env->DeleteLocalRef(classClass);
            env->DeleteLocalRef(cls);

            if (!found) {
                std::cout << "[MAPPER FATAL] No se encontro ningun metodo que retorne " << returnTypeName << " sin argumentos." << std::endl;
            }
        }

        void Mapper::Init() {
            std::cout << "[MAPPER] Inicializando Mapeo Estatico..." << std::endl;

            // 1. CLASES
            ResolveClass(xorstr("Minecraft"), { xorstr("ave"), xorstr("net/minecraft/client/Minecraft") }); // , xorstr("net.minecraft.client.Minecraft")
            ResolveClass(xorstr("EntityPlayerSP"), { xorstr("bew"), xorstr("bud"), xorstr("net/minecraft/client/entity/EntityPlayerSP") });
            ResolveClass(xorstr("MovingObjectType"), { xorstr("auu$a"), xorstr("net/minecraft/util/MovingObjectPosition$MovingObjectType") });

            ResolveClass(xorstr("MovingObjectPosition"), { xorstr("auu"), xorstr("net/minecraft/util/MovingObjectPosition") });

            ResolveClass(xorstr("GuiScreen"), { xorstr("axu"), xorstr("net/minecraft/client/gui/GuiScreen") });


            ResolveClass(xorstr("EntityPlayer"), { xorstr("wn"), xorstr("net/minecraft/entity/player/EntityPlayer") });
            ResolveClass(xorstr("InventoryPlayer"), { xorstr("wm"), xorstr("net/minecraft/entity/player/InventoryPlayer") });
            ResolveClass(xorstr("ItemStack"), { xorstr("zx"), xorstr("net/minecraft/item/ItemStack") });
            ResolveClass(xorstr("Item"), { xorstr("zw"), xorstr("net/minecraft/item/Item") });
            ResolveClass(xorstr("ItemSword"), { xorstr("zy"), xorstr("net/minecraft/item/ItemSword") });
            ResolveClass(xorstr("ItemAxe"), { xorstr("za"), xorstr("net/minecraft/item/ItemAxe") });

            ResolveFieldByIteration(xorstr("EntityPlayer"), xorstr("Player_inventory"),
                { xorstr("bi"), xorstr("inventory"), xorstr("field_71071_by") }
            );

            ResolveMethodByReturnType(xorstr("InventoryPlayer"), xorstr("getCurrentItem"), xorstr("ItemStack"));
            ResolveMethodByReturnType(xorstr("ItemStack"), xorstr("getItem"), xorstr("Item"));

            //ResolveClass(xorstr("EntityPlayer"), { xorstr("wn"), xorstr("net/minecraft/entity/player/EntityPlayer") });
            //ResolveClass(xorstr("InventoryPlayer"), { xorstr("wm"), xorstr("net/minecraft/entity/player/InventoryPlayer") });
            //ResolveClass(xorstr("ItemStack"), { xorstr("net/minecraft/item/ItemStack"), xorstr("zx") });
            //ResolveClass(xorstr("ItemSword"), { xorstr("net/minecraft/item/ItemSword"), xorstr("zy") });
            //ResolveClass(xorstr("ItemAxe"), { xorstr("net/minecraft/item/ItemAxe"), xorstr("za") });
            //ResolveClass(xorstr("Item"), { xorstr("net/minecraft/item/Item"), xorstr("zw"), xorstr("zx"), xorstr("add") });

            //std::string clsInventory = classes[xorstr("InventoryPlayer")];
            //std::string clsItemStack = classes[xorstr("ItemStack")];
            //std::string clsItem = classes[xorstr("Item")];

            //// Firmas construidas
            //std::string sig_InventoryType = "L" + clsInventory + ";";
            //std::string sig_RetItemStack = "()L" + clsItemStack + ";";
            //std::string sig_RetItem = "()L" + clsItem + ";";

            //ResolveField(xorstr("EntityPlayerSP"), xorstr("Player_inventory"), 
            //    { xorstr("bi"), xorstr("inventory"), xorstr("field_71071_by") },
            //    { sig_InventoryType, xorstr("Lnet/minecraft/entity/player/InventoryPlayer;") }
            //);

            //ResolveMethod(xorstr("InventoryPlayer"), xorstr("getCurrentItem"),
            //    { xorstr("h"), xorstr("getCurrentItem") },
            //    sig_RetItemStack
            //);


            //ResolveMethod(xorstr("ItemStack"), xorstr("getItem"),
            //    { xorstr("b"), xorstr("getItem") },
            //    sig_RetItem
            //);



            // thePlayer
            ResolveField(xorstr("Minecraft"), xorstr("Minecraft_thePlayer"),
                { xorstr("h"), xorstr("thePlayer"), xorstr("field_71439_g") },
                { xorstr("Lbew;"), xorstr("Lbud;"), xorstr("Lnet/minecraft/client/entity/EntityPlayerSP;") }
            );

            // objectMouseOver (CORREGIDO CON 't' y 'q')
            ResolveField(xorstr("Minecraft"), xorstr("Minecraft_objectMouseOver"),
                { xorstr("s"), xorstr("r"), xorstr("t"), xorstr("q"), xorstr("objectMouseOver"), xorstr("field_71476_x") },
                { xorstr("Lauu;"), xorstr("Lnet/minecraft/util/MovingObjectPosition;") }
            );

            // currentScreen
            ResolveField(xorstr("Minecraft"), xorstr("Minecraft_currentScreen"),
                { xorstr("m"), xorstr("l"), xorstr("currentScreen"), xorstr("field_71462_r") },
                { xorstr("Laxu;"), xorstr("Lnet/minecraft/client/gui/GuiScreen;") }
            );

            // leftClickCounter
            ResolveField(xorstr("Minecraft"), xorstr("Minecraft_leftClickCounter"),
                { xorstr("f"), xorstr("g"), xorstr("e"), xorstr("leftClickCounter") },
                { xorstr("I") }
            );

            // 3. MÉTODOS
            ResolveMethod(xorstr("Minecraft"), xorstr("clickMouse"),
                { xorstr("aw"), xorstr("av"), xorstr("clickMouse"), xorstr("func_147116_af") },
                xorstr("()V")
            );

            ResolveMethod(xorstr("EntityPlayerSP"), xorstr("getHealth"),
                { xorstr("bn"), xorstr("getHealth"), xorstr("func_110143_aJ") },
                xorstr("()F")
            );

            // Enum Ordinal (Para SmartMining)
            ResolveMethod(xorstr("Enum"), xorstr("ordinal"), { xorstr("ordinal") }, xorstr("()I"));

            ResolveFieldByIteration(xorstr("MovingObjectPosition"), xorstr("MOP_typeOfHit"),
                { xorstr("typeOfHit"), xorstr("a"), xorstr("field_72313_a") }
            );

            std::cout << "[MAPPER] Mapeo Estatico Completado. Sistema Dinamico Listo." << std::endl;
        }
    }
}