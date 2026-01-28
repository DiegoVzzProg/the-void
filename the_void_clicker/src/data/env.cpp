#include "env.h"

namespace Java {

    bool CEnv::Init() {
        // Obtenemos el handle de jvm.dll
        HMODULE hJvm = GetModuleHandleA(xorstr("jvm.dll"));
        if (!hJvm) return false;

        // Obtenemos la dirección de JNI_GetCreatedJavaVMs
        tJNI_GetCreatedJavaVMs pJNI_GetCreatedJavaVMs =
            (tJNI_GetCreatedJavaVMs)GetProcAddress(hJvm, xorstr("JNI_GetCreatedJavaVMs"));

        if (!pJNI_GetCreatedJavaVMs) return false;

        // Le pedimos a la JVM que nos dé su puntero
        jsize vmCount;
        // Nota: 'vm' ahora es una variable de la clase, no necesitamos CEnv::vm
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
        // Usamos 'env' de la instancia
        if (!env) return;

        LOG_INFO(xorstr("[THE VOID] Arreglando ClassLoader..."));

        // 1. Obtener la clase Thread
        jclass threadCls = env->FindClass(xorstr("java/lang/Thread"));

        // 2. Obtener el método estático 'getAllStackTraces'
        jmethodID getAllStackTracesMid = env->GetStaticMethodID(threadCls, xorstr("getAllStackTraces"), xorstr("()Ljava/util/Map;"));
        jobject mapObj = env->CallStaticObjectMethod(threadCls, getAllStackTracesMid);

        // 3. Obtener el Set de llaves
        jclass mapCls = env->GetObjectClass(mapObj);
        jmethodID keySetMid = env->GetMethodID(mapCls, xorstr("keySet"), xorstr("()Ljava/util/Set;"));
        jobject setObj = env->CallObjectMethod(mapObj, keySetMid);

        // 4. Iterar sobre el Set
        jclass setCls = env->GetObjectClass(setObj);
        jmethodID iteratorMid = env->GetMethodID(setCls, xorstr("iterator"), xorstr("()Ljava/util/Iterator;"));
        jobject iteratorObj = env->CallObjectMethod(setObj, iteratorMid);

        jclass iteratorCls = env->GetObjectClass(iteratorObj);
        jmethodID hasNextMid = env->GetMethodID(iteratorCls, xorstr("hasNext"), xorstr("()Z"));
        jmethodID nextMid = env->GetMethodID(iteratorCls, xorstr("next"), xorstr("()Ljava/lang/Object;"));

        jmethodID getNameMid = env->GetMethodID(threadCls, xorstr("getName"), xorstr("()Ljava/lang/String;"));
        jmethodID getContextClassLoaderMid = env->GetMethodID(threadCls, xorstr("getContextClassLoader"), xorstr("()Ljava/lang/ClassLoader;"));
        jmethodID setContextClassLoaderMid = env->GetMethodID(threadCls, xorstr("setContextClassLoader"), xorstr("(Ljava/lang/ClassLoader;)V"));
        jmethodID currentThreadMid = env->GetStaticMethodID(threadCls, xorstr("currentThread"), xorstr("()Ljava/lang/Thread;"));

        bool found = false;

        while (env->CallBooleanMethod(iteratorObj, hasNextMid)) {
            jobject threadObj = env->CallObjectMethod(iteratorObj, nextMid);

            // Obtener nombre del hilo
            jstring nameStr = (jstring)env->CallObjectMethod(threadObj, getNameMid);
            const char* nameChars = env->GetStringUTFChars(nameStr, nullptr);
            std::string name(nameChars);
            env->ReleaseStringUTFChars(nameStr, nameChars);

            // BUSCAMOS EL HILO PRINCIPAL
            if (name == xorstr("Client thread") || name == xorstr("main")) {
                
                LOG_INFO(xorstr("[THE VOID] Hilo Principal Encontrado: ") + name);

                // 1. Obtener el ClassLoader del juego
                jobject loader = env->CallObjectMethod(threadObj, getContextClassLoaderMid);

                // 2. GUARDARLO GLOBALMENTE EN LA INSTANCIA
                if (loader) {
                    ClassLoaderObj = env->NewGlobalRef(loader);
                    LOG_INFO(xorstr("[THE VOID] ClassLoader CAPTURADO y guardado."));
                }

                // 3. Asignarlo al hilo actual
                jobject currentThreadObj = env->CallStaticObjectMethod(threadCls, currentThreadMid);
                env->CallVoidMethod(currentThreadObj, setContextClassLoaderMid, loader);

                found = true;
                break;
            }

            env->DeleteLocalRef(threadObj);
            env->DeleteLocalRef(nameStr);
        }

        if (!found) {
            LOG_ERROR(xorstr("[ERROR FATAL] No se encontro el 'Client thread'. El ClassLoader seguira roto."));
        }

        // Limpieza masiva
        env->DeleteLocalRef(threadCls);
        env->DeleteLocalRef(mapObj);
        env->DeleteLocalRef(mapCls);
        env->DeleteLocalRef(setObj);
        env->DeleteLocalRef(setCls);
        env->DeleteLocalRef(iteratorObj);
        env->DeleteLocalRef(iteratorCls);
    }

    void CEnv::Cleanup() {
        // Limpiamos la referencia global usando el 'env' actual
        if (env && ClassLoaderObj) {
            env->DeleteGlobalRef(ClassLoaderObj);
            ClassLoaderObj = nullptr;
            LOG_INFO(xorstr("[THE VOID] CEnv Limpiado."));
        }
    }
}