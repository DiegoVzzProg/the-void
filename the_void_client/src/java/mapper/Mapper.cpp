#include "Mapper.h"
 
namespace TheVoid {
    namespace Java {
        std::unordered_map<std::string, std::string> Mapper::classes;
        std::unordered_map<std::string, std::string> Mapper::methods;
        std::unordered_map<std::string, std::string> Mapper::fields;
        std::unordered_map<std::string, std::string> Mapper::fieldSigs;
        std::unordered_map<std::string, std::string> Mapper::methodSigs;
        std::unordered_map<std::string, jclass> Mapper::classCache;

        bool Mapper::LearnAndCheck(jobject obj, std::string cacheKey, std::vector<std::string> searchPatterns) {
            if (!env || !obj) return false;

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

        // En Mapper.cpp

        void Mapper::FindRealMOPClass() {
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

            jclass classClass = env->GetObjectClass(mcClass);
            jmethodID getDeclaredField = env->GetMethodID(classClass, "getDeclaredField", "(Ljava/lang/String;)Ljava/lang/reflect/Field;");
            jclass fieldClass = env->FindClass("java/lang/reflect/Field");
            jmethodID getType = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
            jclass classOfClass = env->FindClass("java/lang/Class");
            jmethodID getName = env->GetMethodID(classOfClass, "getName", "()Ljava/lang/String;");

            for (const auto& fieldName : candidates) {
                jstring nameStr = env->NewStringUTF(fieldName.c_str());
                jobject fieldObj = env->CallObjectMethod(mcClass, getDeclaredField, nameStr);
                env->DeleteLocalRef(nameStr);
                env->ExceptionClear();

                if (fieldObj) {
                    // ¡Eureka! Encontramos el campo. ¿De qué clase es?
                    jobject typeObj = env->CallObjectMethod(fieldObj, getType);
                    jstring typeNameStr = (jstring)env->CallObjectMethod(typeObj, getName);
                    const char* typeNameChars = env->GetStringUTFChars(typeNameStr, 0);
                    std::string realMOPClass(typeNameChars);
                    env->ReleaseStringUTFChars(typeNameStr, typeNameChars);

                    std::cout << "[MAPPER DETECTIVE] MovingObjectPosition corregido: " << classes[xorstr("MovingObjectPosition")] << " -> " << realMOPClass << std::endl;

                    // CORREGIMOS EL MAPPER
                    classes[xorstr("MovingObjectPosition")] = realMOPClass;
                    // Asumimos que el Enum está dentro (Clase$a o Clase$MovingObjectType)
                    classes[xorstr("MovingObjectType")] = realMOPClass + "$MovingObjectType";

                    env->DeleteLocalRef(typeObj);
                    env->DeleteLocalRef(fieldObj);
                    break;
                }
            }
            env->DeleteLocalRef(classClass);
            env->DeleteLocalRef(fieldClass);
            env->DeleteLocalRef(classOfClass);
            env->DeleteLocalRef(mcClass);
            env->ExceptionClear();
        }

        void Mapper::Init() {
            std::cout << "[MAPPER] Inicializando Mapeo Estatico..." << std::endl;

            // 1. CLASES
            TheVoid::Helper::Basic::ResolveClass(xorstr("Minecraft"), { xorstr("ave"), xorstr("net/minecraft/client/Minecraft") }); // , xorstr("net.minecraft.client.Minecraft")
            TheVoid::Helper::Basic::ResolveClass(xorstr("EntityPlayerSP"), { xorstr("bew"), xorstr("bud"), xorstr("biz"), xorstr("net/minecraft/client/entity/EntityPlayerSP") });
            
            TheVoid::Helper::Basic::ResolveClass(xorstr("MovingObjectType"), { xorstr("auu$a"), xorstr("net/minecraft/util/MovingObjectPosition$MovingObjectType") });

            TheVoid::Helper::Basic::ResolveClass(xorstr("MovingObjectPosition"), { xorstr("auu"), xorstr("net/minecraft/util/MovingObjectPosition") });
            TheVoid::Helper::Basic::ResolveClass(xorstr("GuiScreen"), { xorstr("axu"), xorstr("net/minecraft/client/gui/GuiScreen") });

            TheVoid::Helper::Basic::ResolveClass(xorstr("EntityPlayer"), { xorstr("wn"), xorstr("net/minecraft/entity/player/EntityPlayer") });
            TheVoid::Helper::Basic::ResolveClass(xorstr("InventoryPlayer"), { xorstr("wm"), xorstr("net/minecraft/entity/player/InventoryPlayer") });
            TheVoid::Helper::Basic::ResolveClass(xorstr("ItemStack"), { xorstr("zx"), xorstr("net/minecraft/item/ItemStack") });
            TheVoid::Helper::Basic::ResolveClass(xorstr("Item"), { xorstr("zw"), xorstr("net/minecraft/item/Item") });
            TheVoid::Helper::Basic::ResolveClass(xorstr("ItemSword"), { xorstr("net/minecraft/item/ItemSword"), xorstr("zy") });
            TheVoid::Helper::Basic::ResolveClass(xorstr("ItemAxe"), { xorstr("net/minecraft/item/ItemAxe"), xorstr("za") });
            TheVoid::Helper::Basic::ResolveClass(xorstr("Entity"), { xorstr("pk"), xorstr("net/minecraft/entity/Entity") });
            TheVoid::Helper::Basic::ResolveClass(std::string(xorstr("EntityLivingBase")), { std::string(xorstr("pr")), std::string(xorstr("net/minecraft/entity/EntityLivingBase")) });

            FindRealMOPClass();

            // Fields
            {
                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("EntityPlayer"), xorstr("Player_inventory"),
                    { xorstr("bi"), xorstr("inventory"), xorstr("field_71071_by") });

                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("Minecraft_thePlayer"),
                    { xorstr("h"), xorstr("thePlayer"), xorstr("field_71439_g") });

                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("Minecraft_objectMouseOver"),
                    { xorstr("s"), xorstr("r"), xorstr("t"), xorstr("q"), xorstr("objectMouseOver"), xorstr("field_71476_x") }, xorstr("MovingObjectPosition"));
         
                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("Minecraft_currentScreen"),
                    { xorstr("m"), xorstr("l"), xorstr("currentScreen"), xorstr("field_71462_r") }, xorstr("GuiScreen"));
            
                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("Minecraft_leftClickCounter"),
                    { xorstr("f"), xorstr("g"), xorstr("e"), xorstr("leftClickCounter") });

                TheVoid::Helper::Expert::ResolveFieldGeneric(
                    xorstr("MovingObjectPosition"),
                    xorstr("MOP_entityHit"),
                    { xorstr("entityHit"), xorstr("d") },
                    xorstr("Entity")
                );


                TheVoid::Helper::Expert::ResolveFieldGeneric(
                    xorstr("EntityLivingBase"),
                    xorstr("ELB_hurtTime"),
                    { xorstr("hurtTime"), xorstr("au"), xorstr("field_70737_aN") }, 
                    "int" 
                );
                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_rotationYaw"), { xorstr("rotationYaw"), xorstr("y") }, "F");
                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_rotationPitch"), { xorstr("rotationPitch"), xorstr("z") }, "F");
                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_width"), { xorstr("width"), xorstr("f") }, "F");
                TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_height"), { xorstr("height"), xorstr("g") }, "F");
                TheVoid::Helper::Expert::ResolveFieldGeneric(
                    xorstr("MovingObjectPosition"),
                    xorstr("MOP_typeOfHit"),
                    { xorstr("typeOfHit"), xorstr("a"), xorstr("field_72313_a") },
                    xorstr("MovingObjectType")
                );
            }


            // [Mapper.cpp -> Init()]

            // --- 1. GAME SETTINGS (Sensibilidad) ---
            // La clase GameSettings. En 1.8 vanilla es 'avh'
            TheVoid::Helper::Basic::ResolveClass(xorstr("GameSettings"), {
                xorstr("net/minecraft/client/settings/GameSettings"),
                xorstr("avh")
                });

            // El campo 'gameSettings' dentro de Minecraft
            TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Minecraft"), xorstr("MC_gameSettings"), {
                xorstr("gameSettings"),
                xorstr("t")
                }, xorstr("GameSettings")); // Firma: Lnet/minecraft/client/settings/GameSettings;

            TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("GameSettings"), xorstr("GS_mouseSensitivity"), {
                xorstr("mouseSensitivity"),
                xorstr("a")
                }, "F");

            // --- 2. ENTITY ROTATIONS & EYES ---
            // Rotaciones (Yaw/Pitch)
            TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_rotationYaw"), { xorstr("rotationYaw"), xorstr("y") }, "F");
            TheVoid::Helper::Expert::ResolveFieldGeneric(xorstr("Entity"), xorstr("Entity_rotationPitch"), { xorstr("rotationPitch"), xorstr("z") }, "F");

            // Altura de los ojos (Para apuntar a la cabeza, no a los pies)
            TheVoid::Helper::Expert::ResolveMethodGeneric(xorstr("Entity"), xorstr("Entity_getEyeHeight"), { xorstr("getEyeHeight"), xorstr("aR") }, "()F");

            // Visibilidad (RayTrace interno del juego - Opcional pero recomendado para Legit)
            TheVoid::Helper::Expert::ResolveMethodGeneric(xorstr("Entity"), xorstr("Entity_canEntityBeSeen"), { xorstr("canEntityBeSeen"), xorstr("n") }, "(Lnet/minecraft/entity/Entity;)Z");

            // Methods
            {
                TheVoid::Helper::Expert::ResolveMethodGeneric(xorstr("InventoryPlayer"), xorstr("getCurrentItem"),
                    { xorstr("getCurrentItem"), xorstr("h") },
                    xorstr("ItemStack"),
                    0
                );
                TheVoid::Helper::Expert::ResolveMethodGeneric(xorstr("ItemStack"), xorstr("getItem"),
                    { xorstr("getItem"), xorstr("b") }, // 'b' es el nombre en 1.8.9 Vanilla
                    xorstr("Item"), 0
                );
                TheVoid::Helper::Expert::ResolveMethodGeneric(xorstr("Minecraft"), xorstr("clickMouse"),
                    { xorstr("aw"), xorstr("av"), xorstr("clickMouse"), xorstr("func_147116_af") },
                    "void",
                    0
                );
                TheVoid::Helper::Expert::ResolveMethodGeneric(xorstr("EntityPlayer"), xorstr("getHealth"),
                    { xorstr("getHealth"), xorstr("bn"), xorstr("func_110143_aJ") },
                    "",
                    0
                );
                TheVoid::Helper::Expert::ResolveMethodGeneric(xorstr("Enum"), xorstr("ordinal"),
                    { xorstr("ordinal") },
                    "int",
                    0
                );
            }

            std::cout << "[MAPPER] Mapeo Estatico Completado. Sistema Dinamico Listo." << std::endl;
        }
    }
}