#include "helperBasic.h"

namespace TheVoid {
	namespace Helper {

        void Basic::ResolveClass(std::string internalName, std::vector<std::string> candidates) {
            std::cout << "[DEBUG] Buscando Clase: " << internalName << "..." << std::endl;
            for (const auto& name : candidates) {
                jclass cls = Utils::FindClass(name.c_str());
                if (cls) {
                    TheVoid::Java::Mapper::classes[internalName] = name;
                    TheVoid::Java::env->DeleteLocalRef(cls);
                    std::cout << "   [OK] Encontrada como: " << name << std::endl;
                    return;
                }
                TheVoid::Java::env->ExceptionClear();
            }
            std::cout << "   [FALLO FATAL] No se encontro la clase: " << internalName << std::endl;
        }

        void Basic::ResolveField(std::string classKey, std::string fieldKey, std::vector<std::string> candidates, std::vector<std::string> sigCandidates) {
            // SEGURIDAD 1: ¿Resolvimos la clase antes?
            if (TheVoid::Java::Mapper::classes.find(classKey) 
                == TheVoid::Java::Mapper::classes.end()) {
                std::cout << "[SKIP] Saltando campo '" << fieldKey << "' porque la clase '" << classKey << "' no existe." << std::endl;
                return;
            }

            std::string realClassName = TheVoid::Java::Mapper::classes[classKey];
            if (realClassName.empty()) return; // SEGURIDAD 2: Cadena vacía

            jclass cls = Utils::FindClass(realClassName.c_str());
            if (!cls) { TheVoid::Java::env->ExceptionClear(); return; }

            for (const auto& name : candidates) {
                for (const auto& sig : sigCandidates) {
                    jfieldID fid = TheVoid::Java::env->GetFieldID(cls, name.c_str(), sig.c_str());
                    if (fid) {
                        TheVoid::Java::Mapper::fields[fieldKey] = name;
                        TheVoid::Java::Mapper::fieldSigs[fieldKey] = sig;
                        TheVoid::Java::env->DeleteLocalRef(cls);
                        // Log silencioso para no spamear, descomenta si quieres ver todo
                        std::cout << "   [FIELD OK] " << fieldKey << " -> " << name << std::endl;
                        return;
                    }
                    TheVoid::Java::env->ExceptionClear();
                }
            }
            TheVoid::Java::env->DeleteLocalRef(cls);
            std::cout << "   [FIELD FALLO] No se encontro campo: " << fieldKey << std::endl;
        }

        void Basic::ResolveMethod(std::string classKey, std::string methodKey, std::vector<std::string> candidates, std::string signature) {
            // SEGURIDAD: Mismo chequeo que en fields
            if (TheVoid::Java::Mapper::classes.find(classKey) == TheVoid::Java::Mapper::classes.end()) return;

            jclass cls = Utils::FindClass(TheVoid::Java::Mapper::classes[classKey].c_str());
            if (!cls) { TheVoid::Java::env->ExceptionClear(); return; }

            for (const auto& name : candidates) {
                jmethodID mid = TheVoid::Java::env->GetMethodID(cls, name.c_str(), signature.c_str());
                if (mid) {
                    TheVoid::Java::Mapper::methods[methodKey] = name;
                    TheVoid::Java::env->DeleteLocalRef(cls);
                    return;
                }
                TheVoid::Java::env->ExceptionClear();
            }
            TheVoid::Java::env->DeleteLocalRef(cls);
            std::cout << "   [METHOD FALLO] No se encontro metodo: " << methodKey << std::endl;
        }
	}
}