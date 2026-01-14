#pragma once
#include <jni.h>
#include <thread>
#include <chrono>
#include <windows.h>

#include "../utils/xorstr.h"

// Definimos el tipo de función para JNI_GetCreatedJavaVMs
typedef jint(JNICALL* tJNI_GetCreatedJavaVMs)(JavaVM**, jsize, jsize*);

namespace TheVoid {
    namespace Java {

        inline JavaVM* vm = nullptr;
        inline JNIEnv* env = nullptr;
        inline bool initialized = false;

        // ESTA ES LA PIEZA QUE FALTABA
        inline bool Init() {
            // 1. Buscamos dónde está cargada la jvm.dll en el proceso
            HMODULE hJvm = GetModuleHandleA(xorstr("jvm.dll"));
            if (!hJvm) return false;

            // 2. Obtenemos la dirección de la función JNI_GetCreatedJavaVMs
            tJNI_GetCreatedJavaVMs pJNI_GetCreatedJavaVMs =
                (tJNI_GetCreatedJavaVMs)GetProcAddress(hJvm, xorstr("JNI_GetCreatedJavaVMs"));

            if (!pJNI_GetCreatedJavaVMs) return false;

            // 3. Le pedimos a la JVM que nos dé su puntero
            jsize vmCount;
            if (pJNI_GetCreatedJavaVMs(&vm, 1, &vmCount) != JNI_OK || vmCount == 0) {
                return false;
            }

            return true;
        }

        inline bool EnsureAttached() {
            if (!vm) return false;

            JNIEnv* temp_env;
            int getEnvStat = vm->GetEnv((void**)&temp_env, JNI_VERSION_1_8);

            if (getEnvStat == JNI_EDETACHED) {
                if (vm->AttachCurrentThread((void**)&temp_env, nullptr) != 0) {
                    return false;
                }
            }
            else if (getEnvStat == JNI_OK) {
                env = temp_env;
                return true;
            }
            else if (getEnvStat == JNI_EVERSION) {
                return false;
            }

            env = temp_env;
            return true;
        }
    }
}