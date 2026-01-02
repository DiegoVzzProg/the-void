#include "java.h"
#include "../patcher/data.h"

JavaVM* vm = nullptr;
jmethodID mid_findClass = nullptr;

void setupClassLoader()
{
    // Obtenemos las clases básicas necesarias para buscar hilos y loaders
    jclass c_Thread = Java::Env->FindClass("java/lang/Thread");
    jclass c_Map = Java::Env->FindClass("java/util/Map");
    jclass c_Set = Java::Env->FindClass("java/util/Set");
    jclass c_ClassLoader = Java::Env->FindClass("java/lang/ClassLoader");

    jmethodID mid_getAllStackTraces = Java::Env->GetStaticMethodID(c_Thread, "getAllStackTraces", "()Ljava/util/Map;");
    jmethodID mid_keySet = Java::Env->GetMethodID(c_Map, "keySet", "()Ljava/util/Set;");
    jmethodID mid_toArray = Java::Env->GetMethodID(c_Set, "toArray", "()[Ljava/lang/Object;");
    jmethodID mid_getContextClassLoader = Java::Env->GetMethodID(c_Thread, "getContextClassLoader", "()Ljava/lang/ClassLoader;");

    // Guardamos este ID para usarlo en AssignClass más tarde
    mid_findClass = Java::Env->GetMethodID(c_ClassLoader, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jobject obj_stackTracesMap = Java::Env->CallStaticObjectMethod(c_Thread, mid_getAllStackTraces);
    jobject obj_threadsSet = Java::Env->CallObjectMethod(obj_stackTracesMap, mid_keySet);

    jobjectArray threads = (jobjectArray)Java::Env->CallObjectMethod(obj_threadsSet, mid_toArray);
    jint szThreads = Java::Env->GetArrayLength(threads);

    for (int i = 0; i < szThreads; i++)
    {
        jobject thread = Java::Env->GetObjectArrayElement(threads, i);
        jobject classLoaderObj = Java::Env->CallObjectMethod(thread, mid_getContextClassLoader);

        if (classLoaderObj) {
            // Verificamos si este Loader conoce la clase Minecraft (para saber si es el correcto)
            jstring className = Java::Env->NewStringUTF("net.minecraft.client.Minecraft");
            jobject minecraftClass = Java::Env->CallObjectMethod(classLoaderObj, mid_findClass, className);

            if (minecraftClass)
            {
                // [CORRECCIÓN CRÍTICA]
                // Convertimos la referencia local a GLOBAL. 
                // Esto permite usar 'Java::classLoader' en Reach, Velocity, Patcher, etc.
                Java::classLoader = Java::Env->NewGlobalRef(classLoaderObj);

                Java::Env->DeleteLocalRef(minecraftClass);
                Java::Env->DeleteLocalRef(className);
                Java::Env->DeleteLocalRef(classLoaderObj); // Borramos la local, ya tenemos la global
                Java::Env->DeleteLocalRef(thread);
                break; // Ya lo encontramos, salimos del bucle
            }
            Java::Env->DeleteLocalRef(className);
        }

        Java::Env->DeleteLocalRef(thread);
        if (classLoaderObj) Java::Env->DeleteLocalRef(classLoaderObj);
    }

    // Limpieza de memoria (Vital en C++)
    Java::Env->DeleteLocalRef(threads);
    Java::Env->DeleteLocalRef(obj_stackTracesMap);
    Java::Env->DeleteLocalRef(obj_threadsSet);
    Java::Env->DeleteLocalRef(c_Thread);
    Java::Env->DeleteLocalRef(c_Map);
    Java::Env->DeleteLocalRef(c_Set);
    Java::Env->DeleteLocalRef(c_ClassLoader);

    // Si encontramos el loader, estamos listos
    if (Java::classLoader != nullptr) {
        Java::Initialized = true;
    }
}

void Java::Init()
{
    Java::Initialized = false;

    // Obtener la JVM existente
    jsize count;
    if (JNI_GetCreatedJavaVMs(&vm, 1, &count) != JNI_OK || count == 0)
        return;

    jint res = vm->GetEnv((void**)&Java::Env, JNI_VERSION_1_6);

    if (res == JNI_EDETACHED)
        res = vm->AttachCurrentThread((void**)&Java::Env, nullptr);

    if (res != JNI_OK || Java::Env == nullptr)
        return;

    vm->GetEnv((void**)&Java::tiEnv, JVMTI_VERSION);
    setupClassLoader();
}

void Java::Kill()
{
    // Liberamos la referencia global para evitar memory leaks al cerrar el cheat
    if (Java::classLoader) {
        Java::Env->DeleteGlobalRef(Java::classLoader);
    }
    vm->DetachCurrentThread();
}

bool Java::AssignClass(std::string name, jclass &out)
{
    if (!Java::classLoader) return false;

    jstring className = Java::Env->NewStringUTF(name.c_str());
    jobject findClass = Java::Env->CallObjectMethod(Java::classLoader, mid_findClass, className);
    Java::Env->DeleteLocalRef(className);

    if (findClass)
    {
        out = (jclass)findClass;
        return true;
    }

    return false;
}

jclass Java::findClass(JNIEnv* p_env, jvmtiEnv* p_tienv, const std::string& path)
{
    jint class_count = 0;
    jclass* classes = nullptr;
    jclass foundclass = nullptr;
    p_tienv->GetLoadedClasses(&class_count, &classes);
    for (int i = 0; i < class_count; ++i)
    {
        char* signature_buffer = nullptr;
        p_tienv->GetClassSignature(classes[i], &signature_buffer, nullptr);

        if (!signature_buffer) continue; // Seguridad extra

        std::string signature = signature_buffer;
        p_tienv->Deallocate((unsigned char*)signature_buffer);

        if (signature.length() > 2) {
            signature = signature.substr(1);
            signature.pop_back();
        }

        if (signature == path)
        {
            foundclass = (jclass)p_env->NewLocalRef(classes[i]);
            // Nota: No hacemos break porque GetLoadedClasses requiere liberar memoria
        }
        p_env->DeleteLocalRef(classes[i]);
    }
    p_tienv->Deallocate((unsigned char*)classes);
    return foundclass;
}
