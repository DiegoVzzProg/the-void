#pragma once
#pragma once
#include <jni.h>
#include "../env.h"
#include "../mapper/Mapper.h"
#include "./Minecraft.h"

namespace TheVoid {
    namespace SDK {

        class CEntity {
        protected:
            jobject instance; // El puntero al objeto Java (EntityPlayerSP)

        public:
            // Constructor: Recibe el objeto crudo de JNI
            CEntity(jobject obj) : instance(obj) {}

            // Verificación vital: ¿Existe la entidad o es nula?
            bool IsValid() const {
                return instance != nullptr;
            }

            // Obtener el objeto crudo (necesario para JNI)
            jobject GetInstance() const {
                return instance;
            }

            // ==========================================================
            // LÓGICA DE JUEGO (Lectura de Memoria)
            // ==========================================================
            float GetHealth() {
                return CallFloatMethod(xorstr("getHealth"));
            }

            float CallFloatMethod(std::string methodKey) {
                if (!instance || !Java::env) return 0.0f;

                // Check rápido
                if (Java::Mapper::methods.find(methodKey) == Java::Mapper::methods.end()) return 0.0f;

                jclass cls = Java::env->GetObjectClass(instance);
                jmethodID mid = Java::env->GetMethodID(cls,
                    Java::Mapper::methods[methodKey].c_str(),
                    xorstr("()F")
                );

                if (!mid) {
                    Java::env->ExceptionClear();
                    Java::env->DeleteLocalRef(cls);
                    return 0.0f;
                }

                float result = Java::env->CallFloatMethod(instance, mid);
                Java::env->DeleteLocalRef(cls);
                return result;
            }
        };
    }
}