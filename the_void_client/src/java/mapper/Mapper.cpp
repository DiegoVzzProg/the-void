#include "Mapper.h"
 
namespace TheVoid {
    namespace Java {
        std::unordered_map<std::string, std::string> Mapper::classes;
        std::unordered_map<std::string, std::string> Mapper::methods;
        std::unordered_map<std::string, std::string> Mapper::fields;
        std::unordered_map<std::string, std::string> Mapper::fieldSigs;
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
                jclass cls = env->FindClass(name.c_str());
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

            jclass cls = env->FindClass(realClassName.c_str());
            if (!cls) { env->ExceptionClear(); return; }

            for (const auto& name : candidates) {
                for (const auto& sig : sigCandidates) {
                    jfieldID fid = env->GetFieldID(cls, name.c_str(), sig.c_str());
                    if (fid) {
                        fields[fieldKey] = name;
                        fieldSigs[fieldKey] = sig;
                        env->DeleteLocalRef(cls);
                        // Log silencioso para no spamear, descomenta si quieres ver todo
                        // std::cout << "   [FIELD OK] " << fieldKey << " -> " << name << std::endl;
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

            jclass cls = env->FindClass(classes[classKey].c_str());
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

        void Mapper::Init() {
            std::cout << "[MAPPER] Inicializando Mapeo Estatico..." << std::endl;

            // 1. CLASES
            ResolveClass(xorstr("Minecraft"), { xorstr("ave"), xorstr("net/minecraft/client/Minecraft") });
            ResolveClass(xorstr("EntityPlayerSP"), { xorstr("bew"), xorstr("bud"), xorstr("net/minecraft/client/entity/EntityPlayerSP") });
            ResolveClass(xorstr("MovingObjectPosition"), { xorstr("auu"), xorstr("net/minecraft/util/MovingObjectPosition") });
            ResolveClass(xorstr("GuiScreen"), { xorstr("axu"), xorstr("net/minecraft/client/gui/GuiScreen") });
            ResolveClass(xorstr("MovingObjectType"), 
                { 
                    xorstr("auu$a"), 
                    xorstr("net/minecraft/util/MovingObjectPosition$MovingObjectType"),
                }
            );

            // 2. CAMPOS

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

            // 3. Resolvemos el campo 'typeOfHit' dentro de MovingObjectPosition
            // Vanilla: 'a' | Forge: 'typeOfHit'
            ResolveField(xorstr("MovingObjectPosition"), xorstr("MOP_typeOfHit"),
                { xorstr("a"), xorstr("typeOfHit"), xorstr("field_72313_a") },
                { xorstr("Lauu$a;"), xorstr("Lnet/minecraft/util/MovingObjectPosition$MovingObjectType;") }
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

            // Enum Ordinal (Para SmartMining)
            ResolveMethod(xorstr("Enum"), xorstr("ordinal"), { xorstr("ordinal") }, xorstr("()I"));

            std::cout << "[MAPPER] Mapeo Estatico Completado. Sistema Dinamico Listo." << std::endl;
        }
    }
}