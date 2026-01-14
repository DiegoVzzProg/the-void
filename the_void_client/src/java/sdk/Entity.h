#pragma once
#pragma once
#include <jni.h>
#include "../env.h"
#include "../mapper/Mapper.h"

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

            // Obtiene la vida actual del jugador
            float GetHealth() {
                if (!instance) return 0.0f;

                jclass cls = Java::env->GetObjectClass(instance);

                // 1. Verificar qué nombre estamos buscando
                std::string methodName = Java::Mapper::methods[xorstr("getHealth")];
                if (methodName.empty()) {
                    std::cout << "[DEBUG] ERROR: El Mapper no tiene clave para 'getHealth'. Revisa Mapper.cpp" << std::endl;
                    return 0.0f;
                }

                // 2. Intentar obtener el ID del método
                jmethodID mid = Java::env->GetMethodID(cls, methodName.c_str(), xorstr("()F"));

                if (!mid) {
                    // Si falla, imprimimos qué intentamos buscar
                    std::cout << "[DEBUG] Fallo al buscar metodo: " << methodName << " con firma ()F en la clase del jugador." << std::endl;

                    // INTENTO DE RECUPERACIÓN (Truco de Herencia)
                    // A veces JNI se marea con la herencia. EntityPlayerSP hereda de EntityLivingBase.
                    // Vamos a intentar buscarlo directamente en la clase padre si falló.
                    // EntityLivingBase en 1.8.9 Vanilla es "pr"
                    jclass superClass = Java::env->FindClass(xorstr("pr"));
                    if (superClass) {
                        mid = Java::env->GetMethodID(superClass, methodName.c_str(), xorstr("()F"));
                        if (mid) {
                            std::cout << "[DEBUG] EXITO: Metodo encontrado en la clase padre (pr)!" << std::endl;
                            return Java::env->CallFloatMethod(instance, mid);
                        }
                    }

                    return 0.0f;
                }

                return Java::env->CallFloatMethod(instance, mid);
            }
        };
    }
}