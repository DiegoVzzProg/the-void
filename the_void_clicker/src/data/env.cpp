#include "env.h"

namespace Java {
	bool CEnv::Init() {
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

    bool CEnv::EnsureAttached() {
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

    void CEnv::FixClassLoader() {
        if (!CEnv::env) return;
        JNIEnv* env = CEnv::env;

        std::cout << "[THE VOID] Arreglando ClassLoader..." << std::endl;

        // 1. Obtener la clase Thread
        jclass threadCls = env->FindClass("java/lang/Thread");

        // 2. Obtener el método estático 'getAllStackTraces' (Nos da un mapa de todos los hilos)
        jmethodID getAllStackTracesMid = env->GetStaticMethodID(threadCls, "getAllStackTraces", "()Ljava/util/Map;");
        jobject mapObj = env->CallStaticObjectMethod(threadCls, getAllStackTracesMid);

        // 3. Obtener el Set de llaves (los hilos) del mapa
        jclass mapCls = env->GetObjectClass(mapObj);
        jmethodID keySetMid = env->GetMethodID(mapCls, "keySet", "()Ljava/util/Set;");
        jobject setObj = env->CallObjectMethod(mapObj, keySetMid);

        // 4. Iterar sobre el Set
        jclass setCls = env->GetObjectClass(setObj);
        jmethodID iteratorMid = env->GetMethodID(setCls, "iterator", "()Ljava/util/Iterator;");
        jobject iteratorObj = env->CallObjectMethod(setObj, iteratorMid);

        jclass iteratorCls = env->GetObjectClass(iteratorObj);
        jmethodID hasNextMid = env->GetMethodID(iteratorCls, "hasNext", "()Z");
        jmethodID nextMid = env->GetMethodID(iteratorCls, "next", "()Ljava/lang/Object;");

        jmethodID getNameMid = env->GetMethodID(threadCls, "getName", "()Ljava/lang/String;");
        jmethodID getContextClassLoaderMid = env->GetMethodID(threadCls, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
        jmethodID setContextClassLoaderMid = env->GetMethodID(threadCls, "setContextClassLoader", "(Ljava/lang/ClassLoader;)V");
        jmethodID currentThreadMid = env->GetStaticMethodID(threadCls, "currentThread", "()Ljava/lang/Thread;");

        bool found = false;

        while (env->CallBooleanMethod(iteratorObj, hasNextMid)) {
            jobject threadObj = env->CallObjectMethod(iteratorObj, nextMid);

            // Obtener nombre del hilo
            jstring nameStr = (jstring)env->CallObjectMethod(threadObj, getNameMid);
            const char* nameChars = env->GetStringUTFChars(nameStr, nullptr);
            std::string name(nameChars);
            env->ReleaseStringUTFChars(nameStr, nameChars);

            // BUSCAMOS EL HILO PRINCIPAL: "Client thread" (Vanilla/Forge) o "main"
            if (name == "Client thread" || name == "main") { // main es para Lunar a veces
                std::cout << "[THE VOID] Hilo Principal Encontrado: " << name << std::endl;

                // 1. Obtener el ClassLoader del juego
                jobject loader = env->CallObjectMethod(threadObj, getContextClassLoaderMid);

                // 2. GUARDARLO GLOBALMENTE (Esto es lo que faltaba)
                if (loader) {
                    ClassLoaderObj = env->NewGlobalRef(loader);
                    std::cout << "[THE VOID] ClassLoader CAPTURADO y guardado." << std::endl;
                }

                // 3. Asignarlo al hilo actual (por si acaso ayuda en algo)
                jobject currentThreadObj = env->CallStaticObjectMethod(threadCls, currentThreadMid);
                env->CallVoidMethod(currentThreadObj, setContextClassLoaderMid, loader);

                found = true;
                break;
            }

            env->DeleteLocalRef(threadObj);
            env->DeleteLocalRef(nameStr);
        }

        if (!found) {
            std::cout << "[ERROR FATAL] No se encontro el 'Client thread'. El ClassLoader seguira roto." << std::endl;
        }

        // Limpieza masiva (Importante)
        env->DeleteLocalRef(threadCls);
        env->DeleteLocalRef(mapObj);
        env->DeleteLocalRef(mapCls);
        env->DeleteLocalRef(setObj);
        env->DeleteLocalRef(setCls);
        env->DeleteLocalRef(iteratorObj);
        env->DeleteLocalRef(iteratorCls);
    }
}