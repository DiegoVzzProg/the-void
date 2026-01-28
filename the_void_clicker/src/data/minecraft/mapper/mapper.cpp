#include "Mapper.h"
 
namespace Java {

    bool CMapper::LearnAndCheck(jobject obj, std::string cacheKey, std::vector<std::string> searchPatterns) {
        if (!Java::CEnv::Get().env || !obj) return false;

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

                LOG_INFO(xorstr("[MAPPER] Aprendido: ") + cacheKey + xorstr(" (") + pattern + xorstr(") detectado como -> ") + realName);

                // IMPORTANTE: Convertir a GlobalRef para que no se borre
                jclass localCls = Java::CEnv::Get().env->GetObjectClass(obj);
                jclass globalCls = (jclass)Java::CEnv::Get().env->NewGlobalRef(localCls);

                // Guardar en Caché
                classCache[cacheKey] = globalCls;

                Java::CEnv::Get().env->DeleteLocalRef(localCls);
                return true;
            }
        }
        return false;
    }

    // En Mapper.cpp

    void CMapper::FindRealMOPClass() {
        // 1. Obtener clase Minecraft
        std::string mcName = classes[xorstr("Minecraft")];
        jclass mcClass = Utils::FindClass(mcName.c_str());
        if (!mcClass) return;

        // 2. Buscamos el campo 'objectMouseOver' (s) en Minecraft
        // Este campo contiene el objeto MOP real.
        std::vector<std::string> candidates = {
            std::string(xorstr("s")),
            std::string(xorstr("objectMouseOver"))
        };

        jclass classClass = Java::CEnv::Get().env->GetObjectClass(mcClass);
        jmethodID getDeclaredField = Java::CEnv::Get().env->GetMethodID(classClass, xorstr("getDeclaredField"), xorstr("(Ljava/lang/String;)Ljava/lang/reflect/Field;"));
        jclass fieldClass = Java::CEnv::Get().env->FindClass(xorstr("java/lang/reflect/Field"));
        jmethodID getType = Java::CEnv::Get().env->GetMethodID(fieldClass, xorstr("getType"), xorstr("()Ljava/lang/Class;"));
        jclass classOfClass = Java::CEnv::Get().env->FindClass(xorstr("java/lang/Class"));
        jmethodID getName = Java::CEnv::Get().env->GetMethodID(classOfClass, xorstr("getName"), xorstr("()Ljava/lang/String;"));

        for (const auto& fieldName : candidates) {
            jstring nameStr = Java::CEnv::Get().env->NewStringUTF(fieldName.c_str());
            jobject fieldObj = Java::CEnv::Get().env->CallObjectMethod(mcClass, getDeclaredField, nameStr);
            Java::CEnv::Get().env->DeleteLocalRef(nameStr);
            Java::CEnv::Get().env->ExceptionClear();

            if (fieldObj) {
                // ¡Eureka! Encontramos el campo. ¿De qué clase es?
                jobject typeObj = Java::CEnv::Get().env->CallObjectMethod(fieldObj, getType);
                jstring typeNameStr = (jstring)Java::CEnv::Get().env->CallObjectMethod(typeObj, getName);
                const char* typeNameChars = Java::CEnv::Get().env->GetStringUTFChars(typeNameStr, 0);
                std::string realMOPClass(typeNameChars);
                Java::CEnv::Get().env->ReleaseStringUTFChars(typeNameStr, typeNameChars);

                LOG_INFO(xorstr("[MAPPER DETECTIVE] MovingObjectPosition corregido: ") + classes[xorstr("MovingObjectPosition")] + xorstr(" -> ") + realMOPClass);

                // CORREGIMOS EL MAPPER
                classes[xorstr("MovingObjectPosition")] = realMOPClass;
                // Asumimos que el Enum está dentro (Clase$a o Clase$MovingObjectType)
                classes[xorstr("MovingObjectType")] = realMOPClass + xorstr("$MovingObjectType");

                Java::CEnv::Get().env->DeleteLocalRef(typeObj);
                Java::CEnv::Get().env->DeleteLocalRef(fieldObj);
                break;
            }
        }
        Java::CEnv::Get().env->DeleteLocalRef(classClass);
        Java::CEnv::Get().env->DeleteLocalRef(fieldClass);
        Java::CEnv::Get().env->DeleteLocalRef(classOfClass);
        Java::CEnv::Get().env->DeleteLocalRef(mcClass);
        Java::CEnv::Get().env->ExceptionClear();
    }

    void CMapper::Init() {
        LOG_INFO(xorstr("[MAPPER] Inicializando Mapeo Estatico..."));

        Helper::Basic::ResolveClass(xorstr("GameSettings"), {
            xorstr("net/minecraft/client/settings/GameSettings"),
            xorstr("avh")
        });
        Helper::Basic::ResolveClass(xorstr("Minecraft"), { xorstr("ave"), xorstr("net/minecraft/client/Minecraft") });
        Helper::Basic::ResolveClass(xorstr("EntityPlayerSP"), { xorstr("bew"), xorstr("bud"), xorstr("biz"), xorstr("net/minecraft/client/entity/EntityPlayerSP") });

        Helper::Basic::ResolveClass(xorstr("MovingObjectType"), { xorstr("auu$a"), xorstr("net/minecraft/util/MovingObjectPosition$MovingObjectType") });

        Helper::Basic::ResolveClass(xorstr("MovingObjectPosition"), { xorstr("auu"), xorstr("net/minecraft/util/MovingObjectPosition") });
        Helper::Basic::ResolveClass(xorstr("GuiScreen"), { xorstr("axu"), xorstr("net/minecraft/client/gui/GuiScreen") });

        Helper::Basic::ResolveClass(xorstr("EntityPlayer"), { xorstr("wn"), xorstr("net/minecraft/entity/player/EntityPlayer") });
        Helper::Basic::ResolveClass(xorstr("InventoryPlayer"), { xorstr("wm"), xorstr("net/minecraft/entity/player/InventoryPlayer") });
        Helper::Basic::ResolveClass(xorstr("ItemStack"), { xorstr("zx"), xorstr("net/minecraft/item/ItemStack") });
        Helper::Basic::ResolveClass(xorstr("Item"), { xorstr("zw"), xorstr("net/minecraft/item/Item") });
        Helper::Basic::ResolveClass(xorstr("ItemSword"), { xorstr("net/minecraft/item/ItemSword"), xorstr("zy") });
        Helper::Basic::ResolveClass(xorstr("ItemAxe"), { xorstr("net/minecraft/item/ItemAxe"), xorstr("za") });
        Helper::Basic::ResolveClass(xorstr("Entity"), { xorstr("pk"), xorstr("net/minecraft/entity/Entity") });
        Helper::Basic::ResolveClass(xorstr("EntityLivingBase"), { xorstr("pr"), xorstr("net/minecraft/entity/EntityLivingBase") });
        Helper::Basic::ResolveClass(xorstr("WorldClient"), { xorstr("net/minecraft/client/multiplayer/WorldClient"), xorstr("bdb"), xorstr("cen") });
        Helper::Basic::ResolveClass(xorstr("KeyBinding"), { xorstr("net/minecraft/client/settings/KeyBinding"), xorstr("avb") });
        FindRealMOPClass();

        // Fields
        {
            Helper::Expert::ResolveFieldGeneric(
                xorstr("GameSettings"),
                xorstr("keyBindUseItem"), 
                { xorstr("keyBindUseItem"), xorstr("ap") },
                xorstr("KeyBinding")
            );

            Helper::Expert::ResolveFieldGeneric(
                xorstr("GameSettings"),
                xorstr("keyBindAttack"),
                { xorstr("keyBindAttack"), xorstr("aq") },
                xorstr("KeyBinding")
            );


            Helper::Expert::ResolveFieldGeneric(
                xorstr("KeyBinding"),
                xorstr("pressed"),
                { xorstr("pressed"), xorstr("i") },
                xorstr("Z") 
            );

            Helper::Expert::ResolveFieldGeneric(xorstr("EntityPlayer"), xorstr("Player_inventory"),
                { xorstr("bi"), xorstr("inventory"), xorstr("field_71071_by") });

            Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("Minecraft_thePlayer"),
                { xorstr("h"), xorstr("thePlayer"), xorstr("field_71439_g") });

            Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("Minecraft_objectMouseOver"),
                { xorstr("s"), xorstr("r"), xorstr("t"), xorstr("q"), xorstr("objectMouseOver"), xorstr("field_71476_x") }, xorstr("MovingObjectPosition"));

            Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("Minecraft_currentScreen"),
                { xorstr("m"), xorstr("l"), xorstr("currentScreen"), xorstr("field_71462_r") }, xorstr("GuiScreen"));

            Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("Minecraft_leftClickCounter"),
                { xorstr("f"), xorstr("g"), xorstr("e"), xorstr("leftClickCounter") });

            Helper::Expert::ResolveFieldGeneric(
                xorstr("MovingObjectPosition"),
                xorstr("MOP_entityHit"),
                { xorstr("entityHit"), xorstr("d") },
                xorstr("Entity")
            );


            Helper::Expert::ResolveFieldGeneric(
                xorstr("EntityLivingBase"),
                xorstr("ELB_hurtTime"),
                { xorstr("hurtTime"), xorstr("au"), xorstr("field_70737_aN") },
                xorstr("int")
            );
            Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_rotationYaw"), { xorstr("rotationYaw"), xorstr("y") }, xorstr("F"));
            Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_rotationPitch"), { xorstr("rotationPitch"), xorstr("z") }, xorstr("F"));
            Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_width"), { xorstr("width"), xorstr("f") }, xorstr("F"));
            Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_height"), { xorstr("height"), xorstr("g") }, xorstr("F"));
            Helper::Expert::ResolveFieldGeneric(
                xorstr("MovingObjectPosition"),
                xorstr("MOP_typeOfHit"),
                { xorstr("typeOfHit"), xorstr("a"), xorstr("field_72313_a") },
                xorstr("MovingObjectType")
            );

            Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_posX"), { xorstr("posX"), xorstr("s") }, xorstr("D"));
            Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_posY"), { xorstr("posY"), xorstr("t") }, xorstr("D"));
            Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_posZ"), { xorstr("posZ"), xorstr("u") }, xorstr("D"));

            Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("MC_theWorld"), { xorstr("theWorld"), xorstr("f") }, xorstr("WorldClient"));
            Helper::Expert::ResolveFieldGeneric(
                xorstr("WorldClient"),
                xorstr("World_entityList"),
                {
                  xorstr("loadedEntityList"),
                  xorstr("k"),
                  xorstr("field_72996_f") 
                },
                xorstr("java/util/List")
            );
        }


        // El campo 'gameSettings' dentro de Minecraft
        Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("MC_gameSettings"), {
            xorstr("gameSettings"),
            xorstr("t")
            }, xorstr("GameSettings")); // Firma: Lnet/minecraft/client/settings/GameSettings;

        Helper::Expert::ResolveFieldGeneric(xorstr("GameSettings"), xorstr("GS_mouseSensitivity"), {
            xorstr("mouseSensitivity"),
            xorstr("a")
            }, xorstr("F"));

        // --- 2. ENTITY ROTATIONS & EYES ---
        // Rotaciones (Yaw/Pitch)
        Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_rotationYaw"), { xorstr("rotationYaw"), xorstr("y") }, xorstr("F"));
        Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_rotationPitch"), { xorstr("rotationPitch"), xorstr("z") }, xorstr("F"));

        // Altura de los ojos (Para apuntar a la cabeza, no a los pies)
        Helper::Expert::ResolveMethodGeneric(xorstr("Entity"), xorstr("Entity_getEyeHeight"), { xorstr("getEyeHeight"), xorstr("aR") }, xorstr("()F"));

        // Visibilidad (RayTrace interno del juego - Opcional pero recomendado para Legit)
        Helper::Expert::ResolveMethodGeneric(xorstr("Entity"), xorstr("Entity_canEntityBeSeen"), { xorstr("canEntityBeSeen"), xorstr("n") }, xorstr("(Lnet/minecraft/entity/Entity;)Z"));

        // Methods
        {
            Helper::Expert::ResolveMethodGeneric(xorstr("InventoryPlayer"), xorstr("getCurrentItem"),
                { xorstr("getCurrentItem"), xorstr("h") },
                xorstr("ItemStack"),
                0
            );
            Helper::Expert::ResolveMethodGeneric(xorstr("ItemStack"), xorstr("getItem"),
                { xorstr("getItem"), xorstr("b") }, // 'b' es el nombre en 1.8.9 Vanilla
                xorstr("Item"), 0
            );
            Helper::Expert::ResolveMethodGeneric(xorstr("Minecraft"), xorstr("clickMouse"),
                { xorstr("aw"), xorstr("av"), xorstr("clickMouse"), xorstr("func_147116_af") },
                xorstr("void"),
                0
            );
            Helper::Expert::ResolveMethodGeneric(xorstr("EntityPlayer"), xorstr("getHealth"),
                { xorstr("getHealth"), xorstr("bn"), xorstr("func_110143_aJ") },
                "",
                0
            );
            Helper::Expert::ResolveMethodGeneric(xorstr("Enum"), xorstr("ordinal"),
                { xorstr("ordinal") },
                xorstr("int"),
                0
            );
        }

        LOG_INFO(xorstr("[MAPPER] Mapeo Estatico Completado. Sistema Dinamico Listo."));
    }

    void CMapper::Cleanup() {
        // CORRECCIN 1: Variable esttica para evitar doble ejecucin
        static bool alreadyCleaned = false;
        if (alreadyCleaned) return;
        alreadyCleaned = true;

        // CORRECCIN 2: Arreglamos el typo "Get().Get()" que tenas
        if (!Java::CEnv::Get().env) return;

        for (auto& pair : classCache) {
            if (pair.second) {
                Java::CEnv::Get().env->DeleteGlobalRef(pair.second);
            }
        }

        classCache.clear();
        classes.clear();
        methods.clear();
        fields.clear();

        // Ahora este log es seguro porque ocurre en el paso 1 de Main.cpp
        LOG_INFO(xorstr("[MAPPER] Memoria liberada correctamente."));
    }
}