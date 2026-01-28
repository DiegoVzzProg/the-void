#include "minecraft.h"

namespace Java {
    jobject CMinecraft::GetJObject() {
        return internal_mc_instance;
    }

    // 2. Limpieza de memoria (Evita crasheos al cerrar)
    void CMinecraft::Cleanup() {
        if (internal_mc_instance && Java::CEnv::Get().env) {
            Java::CEnv::Get().env->DeleteGlobalRef(internal_mc_instance);
            internal_mc_instance = nullptr;
            LOG_INFO(xorstr("[THE VOID] Instancia de Minecraft liberada."));
        }
    }

    // 3. Inicialización (Búsqueda inteligente)
    bool CMinecraft::Initialize() {
        // Si ya tenemos la instancia, no buscamos de nuevo
        if (internal_mc_instance) return true;
        if (!Java::CEnv::Get().env) return false;

        struct Candidate {
            std::string className;
            std::string methodName;
            std::string signature;
            bool isMethod;
        };

        // Tu lista de intentos con xorstr (Perfecto)
        std::vector<Candidate> attempts = {
            { xorstr("net/minecraft/client/Minecraft"), xorstr("getMinecraft"), xorstr("()Lnet/minecraft/client/Minecraft;"), true },
            { xorstr("ave"), xorstr("A"), xorstr("()Lave;"), true },
            { xorstr("ave"), xorstr("S"), xorstr("Lave;"), false },
            { xorstr("ave"), xorstr("theMinecraft"), xorstr("Lave;"), false },
            { xorstr("net/minecraft/client/Minecraft"), xorstr("theMinecraft"), xorstr("Lnet/minecraft/client/Minecraft;"), false }
        };

        for (const auto& attempt : attempts) {
            // 1. Buscamos la clase
            jclass cls = Utils::FindClass(attempt.className.c_str());

            if (!cls) {
                Java::CEnv::Get().env->ExceptionClear();
                continue;
            }

            jobject localMcObj = nullptr;

            if (attempt.isMethod) {
                jmethodID mid = Java::CEnv::Get().env->GetStaticMethodID(cls, attempt.methodName.c_str(), attempt.signature.c_str());
                if (mid) {
                    localMcObj = Java::CEnv::Get().env->CallStaticObjectMethod(cls, mid);
                    static bool msg1 = false;
                    if (!msg1) {
                        msg1 = true;
                        LOG_INFO(xorstr("[THE VOID] Instancia encontrada via METODO: ") + attempt.className + xorstr("::") + attempt.methodName);
                    }
                }
            }
            else {
                jfieldID fid = Java::CEnv::Get().env->GetStaticFieldID(cls, attempt.methodName.c_str(), attempt.signature.c_str());
                if (fid) {
                    localMcObj = Java::CEnv::Get().env->GetStaticObjectField(cls, fid);
                    static bool msg2 = false;
                    if (!msg2) {
                        msg2 = true;
                        LOG_INFO(xorstr("[THE VOID] Instancia encontrada via CAMPO: ") + attempt.className + xorstr("::") + attempt.methodName);
                    }
                }
            }

            if (!localMcObj) Java::CEnv::Get().env->ExceptionClear();

            if (localMcObj) {
                // --- ÉXITO ---

                // 1. Guardar en el Mapper qué nombre funcionó
                Java::CMapper::Get().classes[xorstr("Minecraft")] = attempt.className;

                // 2. IMPORTANTE: Convertir a GlobalRef para el Singleton
                internal_mc_instance = Java::CEnv::Get().env->NewGlobalRef(localMcObj);

                // 3. Borrar las referencias locales (ya no las necesitamos porque tenemos la Global)
                Java::CEnv::Get().env->DeleteLocalRef(localMcObj);
                Java::CEnv::Get().env->DeleteLocalRef(cls);

                return true; // Inicialización exitosa
            }

            Java::CEnv::Get().env->DeleteLocalRef(cls);
        }

        static bool msg3 = false;
        if (!msg3) {
            msg3 = true;
            LOG_ERROR(xorstr("[FATAL] No se pudo obtener la instancia de Minecraft en ningun intento."));
        }

        return false; // Falló
    }

    jobject CMinecraft::GetFieldObject(std::string fieldKey) {
        // 1. CHEQUEO: ¿Existe la instancia de Minecraft?
        if (!internal_mc_instance) {
            // Opcional: Descomentar si quieres ver este error (puede ser spam al inicio)
            // LOG_WARN(xorstr("[GetFieldObject] Error: internal_mc_instance es NULL (Juego no listo)."));
            return nullptr;
        }

        // 2. CHEQUEO: ¿Existe la llave en el Mapper (Nombre)?
        if (Java::CMapper::Get().fields.find(fieldKey) == Java::CMapper::Get().fields.end()) {
            LOG_ERROR(xorstr("[GetFieldObject] CRITICO: La key '") + fieldKey + xorstr("' no existe en Mapper::fields."));
            return nullptr;
        }

        // 3. CHEQUEO: ¿Existe la firma en el Mapper (Signature)?
        // Es vital verificar esto para evitar crashes si el mapa está desincronizado
        if (Java::CMapper::Get().fieldSigs.find(fieldKey) == Java::CMapper::Get().fieldSigs.end()) {
            LOG_ERROR(xorstr("[GetFieldObject] CRITICO: La key '") + fieldKey + xorstr("' no tiene firma en Mapper::fieldSigs."));
            return nullptr;
        }

        jclass mcClass = Java::CEnv::Get().env->GetObjectClass(internal_mc_instance);

        const char* fieldName = Java::CMapper::Get().fields[fieldKey].c_str();
        const char* fieldSig = Java::CMapper::Get().fieldSigs[fieldKey].c_str();

        // 4. INTENTO JNI
        jfieldID fid = Java::CEnv::Get().env->GetFieldID(mcClass, fieldName, fieldSig);

        if (!fid) {
            Java::CEnv::Get().env->ExceptionClear(); // Limpiamos la excepción de Java para no crashear
            Java::CEnv::Get().env->DeleteLocalRef(mcClass);

            // 5. REPORTE DE ERROR DETALLADO
            // Esto te dirá exactamente qué nombre y firma fallaron en tiempo real
            std::string errorMsg = xorstr("[GetFieldObject] JNI Fallo. Key: ") + fieldKey +
                xorstr(" | Name: ") + std::string(fieldName) +
                xorstr(" | Sig: ") + std::string(fieldSig);
            LOG_ERROR(errorMsg);

            return nullptr;
        }

        jobject Obj = Java::CEnv::Get().env->GetObjectField(internal_mc_instance, fid);

        Java::CEnv::Get().env->DeleteLocalRef(mcClass); // Limpieza

        // Si llega aquí, todo salió BIEN. Retorna el objeto válido.
        return Obj;
    }
}