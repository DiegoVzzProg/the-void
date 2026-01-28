#include "helperBasic.h"

namespace Helper {

    void Basic::ResolveClass(std::string internalName, std::vector<std::string> candidates) {
        LOG_INFO(xorstr("[DEBUG] Buscando Clase: ") + internalName + xorstr("..."));
        for (const auto& name : candidates) {
            jclass cls = Utils::FindClass(name.c_str());
            if (cls) {
                Java::CMapper::Get().classes[internalName] = name;
                Java::CEnv::Get().env->DeleteLocalRef(cls);
                LOG_INFO(xorstr("   [OK] Encontrada como: ") + name);
                return;
            }
            Java::CEnv::Get().env->ExceptionClear();
        }
        LOG_WARN(xorstr("   [FALLO FATAL] No se encontro la clase: ") + internalName);
    }

    void Basic::ResolveField(std::string classKey, std::string fieldKey, std::vector<std::string> candidates, std::vector<std::string> sigCandidates) {
        // SEGURIDAD 1: ¿Resolvimos la clase antes?
        if (Java::CMapper::Get().classes.find(classKey)
            == Java::CMapper::Get().classes.end()) {
            LOG_INFO(xorstr("[SKIP] Saltando campo '") + fieldKey + xorstr("' porque la clase '") + classKey + xorstr("' no existe."));
            return;
        }

        std::string realClassName = Java::CMapper::Get().classes[classKey];
        if (realClassName.empty()) return; // SEGURIDAD 2: Cadena vacía

        jclass cls = Utils::FindClass(realClassName.c_str());
        if (!cls) { Java::CEnv::Get().env->ExceptionClear(); return; }

        for (const auto& name : candidates) {
            for (const auto& sig : sigCandidates) {
                jfieldID fid = Java::CEnv::Get().env->GetFieldID(cls, name.c_str(), sig.c_str());
                if (fid) {
                    Java::CMapper::Get().fields[fieldKey] = name;
                    Java::CMapper::Get().fieldSigs[fieldKey] = sig;
                    Java::CEnv::Get().env->DeleteLocalRef(cls);
                    // Log silencioso para no spamear, descomenta si quieres ver todo
                    LOG_INFO(xorstr("   [FIELD OK] ") + fieldKey + xorstr(" -> ") + name);
                    return;
                }
                Java::CEnv::Get().env->ExceptionClear();
            }
        }
        Java::CEnv::Get().env->DeleteLocalRef(cls);
        LOG_WARN(xorstr("   [FIELD FALLO] No se encontro campo: ") + fieldKey);
    }

    void Basic::ResolveMethod(std::string classKey, std::string methodKey, std::vector<std::string> candidates, std::string signature) {
        // SEGURIDAD: Mismo chequeo que en fields
        if (Java::CMapper::Get().classes.find(classKey) == Java::CMapper::Get().classes.end()) return;

        jclass cls = Utils::FindClass(Java::CMapper::Get().classes[classKey].c_str());
        if (!cls) { Java::CEnv::Get().env->ExceptionClear(); return; }

        for (const auto& name : candidates) {
            jmethodID mid = Java::CEnv::Get().env->GetMethodID(cls, name.c_str(), signature.c_str());
            if (mid) {
                Java::CMapper::Get().methods[methodKey] = name;
                Java::CEnv::Get().env->DeleteLocalRef(cls);
                return;
            }
            Java::CEnv::Get().env->ExceptionClear();
        }
        Java::CEnv::Get().env->DeleteLocalRef(cls);
        LOG_WARN(xorstr("   [METHOD FALLO] No se encontro metodo: ") + methodKey);
    }
}