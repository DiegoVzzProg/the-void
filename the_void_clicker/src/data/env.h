#pragma once
#include <jni.h>
#include <thread>
#include <chrono>

// --- CONFIGURACIÓN WINDOWS ---
#define WIN32_LEAN_AND_MEAN 
#include <winsock2.h>       
#include <windows.h>
// -----------------------------

#include <iostream>
#include "../dll/debug.h"
#include "./encrypt/encrypt.h"

typedef jint(JNICALL* tJNI_GetCreatedJavaVMs)(JavaVM**, jsize, jsize*);

namespace Java {
    class CEnv {
    private:
        CEnv() = default;
        ~CEnv() { Cleanup(); }

    public:
        // 2. Eliminamos constructores de copia (Regla del Singleton)
        CEnv(const CEnv&) = delete;
        void operator=(const CEnv&) = delete;

        // 3. El método de acceso estático (Singleton)
        static CEnv& Get() {
            static CEnv instance;
            return instance;
        }

        // 4. Variables de Instancia (Ya NO son static)
        // Las dejamos públicas para acceder fácilmente: CEnv::Get().env
        JNIEnv* env = nullptr;
        HWND GameWindow = nullptr;
        JavaVM* vm = nullptr;
        bool initialized = false;
        jobject ClassLoaderObj = nullptr;

        // 5. Métodos de Instancia (Ya NO son static)
        bool Init();
        void Cleanup();
        bool EnsureAttached();
        void FixClassLoader();
    };
}