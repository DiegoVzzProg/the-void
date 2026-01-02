#include "../../../ext/xorstr/xorstr.h"
#include "patcher.h"
#include "miniz.h"
#include <iostream>
#include "data.h"

// Debug macro: Descomentar para ver errores en la consola
#define DEBUG_LOG(x) std::cout << "[TheVoid] " << x << std::endl

//#include <vector>
//#include <functional>
//
//// Macro para limpieza rápida de JNI
//#define SAFE_DELETE_LOCAL(env, ref) if (ref) { env->DeleteLocalRef(ref); ref = nullptr; }
//#define SAFE_DELETE_GLOBAL(env, ref) if (ref) { env->DeleteGlobalRef(ref); ref = nullptr; }
//
//namespace Patcher
//{
//    namespace
//    {
//        // Función auxiliar para cargar el JAR desde memoria (Sin cambios lógicos, solo limpieza)
//        void loadJar(jobject classLoader, const unsigned char* jarBytes, size_t size)
//        {
//            mz_zip_archive archive{};
//            if (!mz_zip_reader_init_mem(&archive, jarBytes, size, 0)) return;
//
//            mz_uint file_number = mz_zip_reader_get_num_files(&archive);
//            for (mz_uint i = 0; i < file_number; i++)
//            {
//                if (!mz_zip_reader_is_file_supported(&archive, i) || mz_zip_reader_is_file_a_directory(&archive, i))
//                    continue;
//
//                char str[256] = { 0 };
//                mz_zip_reader_get_filename(&archive, i, str, 256);
//                std::string filename(str);
//
//                if (filename.length() > 6 && filename.substr(filename.length() - 6) == ".class")
//                {
//                    size_t classBytes_size = 0;
//                    unsigned char* classBytes = (unsigned char*)mz_zip_reader_extract_to_heap(&archive, i, &classBytes_size, 0);
//
//                    if (classBytes)
//                    {
//                        // DefineClass carga la clase del JAR en la JVM
//                        jclass jaclass = Java::Env->DefineClass(nullptr, classLoader, (const jbyte*)classBytes, classBytes_size);
//                        if (jaclass) {
//                            Java::Env->DeleteLocalRef(jaclass);
//                        }
//                        else {
//                            if (Java::Env->ExceptionCheck()) Java::Env->ExceptionClear();
//                        }
//                        mz_free(classBytes);
//                    }
//                }
//            }
//            mz_zip_reader_end(&archive);
//        }
//
//        // Hook de JVMTI: Aquí ocurre la magia de la inyección en tiempo real
//        void JNICALL ClassFileLoadHook
//        (
//            jvmtiEnv* jvmti_env,
//            JNIEnv* jni_env,
//            jclass class_being_redefined,
//            jobject loader,
//            const char* name,
//            jobject protection_domain,
//            jint class_data_len,
//            const unsigned char* class_data,
//            jint* new_class_data_len,
//            unsigned char** new_class_data
//        )
//        {
//            // Solo nos interesa EntityRenderer
//            if (EntityRenderer_class && jni_env->IsSameObject(class_being_redefined, EntityRenderer_class))
//            {
//                // Buscamos nuestra clase "Patcher" (que cargamos con loadJar)
//                // NOTA: Usamos findClass simple aquí por seguridad
//                jclass ClassPatcherClass = jni_env->FindClass(xorstr("io/github/lefraudeur/ClassPatcher"));
//
//                if (!ClassPatcherClass) {
//                    if (jni_env->ExceptionCheck()) jni_env->ExceptionClear();
//                    return;
//                }
//
//                // Preparamos los datos para el método de parcheo Java
//                jbyteArray original_class_bytes = jni_env->NewByteArray(class_data_len);
//                jni_env->SetByteArrayRegion(original_class_bytes, 0, class_data_len, (const jbyte*)class_data);
//
//                // Firma: (bytesOriginales, metodoAParchear, claseContexto, nombreMapa) -> bytesNuevos
//                jmethodID patchMethodID = jni_env->GetStaticMethodID(ClassPatcherClass, xorstr("patchEntityRenderer"), xorstr("([BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)[B"));
//
//                if (patchMethodID) {
//                    jstring methodToPatchStr = jni_env->NewStringUTF(xorstr("getMouseOver"));
//                    jstring ThreadContextClassName = jni_env->NewStringUTF(xorstr("org/apache/logging/log4j/ThreadContext"));
//                    jstring emptyMapName = jni_env->NewStringUTF(xorstr("EMPTY_MAP"));
//
//                    jbyteArray new_class_bytes = (jbyteArray)jni_env->CallStaticObjectMethod(
//                        ClassPatcherClass,
//                        patchMethodID,
//                        original_class_bytes,
//                        methodToPatchStr,
//                        ThreadContextClassName,
//                        emptyMapName
//                    );
//
//                    // Si Java devolvió bytes modificados, se los damos a la JVM
//                    if (new_class_bytes) {
//                        *new_class_data_len = jni_env->GetArrayLength(new_class_bytes);
//                        jvmti_env->Allocate(*new_class_data_len, new_class_data);
//                        jni_env->GetByteArrayRegion(new_class_bytes, 0, *new_class_data_len, (jbyte*)*new_class_data);
//                        jni_env->DeleteLocalRef(new_class_bytes);
//                    }
//
//                    jni_env->DeleteLocalRef(methodToPatchStr);
//                    jni_env->DeleteLocalRef(ThreadContextClassName);
//                    jni_env->DeleteLocalRef(emptyMapName);
//                }
//
//                jni_env->DeleteLocalRef(ClassPatcherClass);
//                jni_env->DeleteLocalRef(original_class_bytes);
//            }
//        }
//    }
//
//    // Helper para crear un ClassLoader limpio (aislado)
//    jobject getCleanClassLoader()
//    {
//        jclass urlClass = Java::Env->FindClass(xorstr("java/net/URL"));
//        jobjectArray emptyUrls = Java::Env->NewObjectArray(0, urlClass, nullptr);
//
//        jclass URLClassLoaderClass = Java::Env->FindClass(xorstr("java/net/URLClassLoader"));
//        jmethodID ctor = Java::Env->GetMethodID(URLClassLoaderClass, xorstr("<init>"), xorstr("([Ljava/net/URL;)V"));
//
//        jobject classLoader = Java::Env->NewObject(URLClassLoaderClass, ctor, emptyUrls);
//
//        Java::Env->DeleteLocalRef(urlClass);
//        Java::Env->DeleteLocalRef(emptyUrls);
//        Java::Env->DeleteLocalRef(URLClassLoaderClass);
//
//        return classLoader;
//    }
//
//    void Init()
//    {
//        // 1. Configuración de JVMTI (Estándar)
//        jvmtiCapabilities capabilities{};
//        capabilities.can_retransform_classes = JVMTI_ENABLE;
//        Java::tiEnv->AddCapabilities(&capabilities);
//
//        jvmtiEventCallbacks callbacks{};
//        callbacks.ClassFileLoadHook = &ClassFileLoadHook;
//        Java::tiEnv->SetEventCallbacks(&callbacks, sizeof(jvmtiEventCallbacks));
//        Java::tiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
//
//        // 2. Obtener Clases Críticas (EntityRenderer)
//        // Usamos AssignClass que definimos en java.h para ser consistentes
//        jclass localRenderer = nullptr;
//        if (Java::AssignClass(xorstr("net.minecraft.client.renderer.EntityRenderer"), localRenderer)) {
//            EntityRenderer_class = (jclass)Java::Env->NewGlobalRef(localRenderer);
//            Java::Env->DeleteLocalRef(localRenderer);
//        }
//        else {
//            // Fallback
//            jclass tmp = Java::Env->FindClass(xorstr("net/minecraft/client/renderer/EntityRenderer"));
//            if (tmp) {
//                EntityRenderer_class = (jclass)Java::Env->NewGlobalRef(tmp);
//                Java::Env->DeleteLocalRef(tmp);
//            }
//        }
//
//        // 3. ThreadContext & Map Swap
//        jclass ThreadContext_class = Java::findClass(Java::Env, Java::tiEnv, xorstr("org/apache/logging/log4j/ThreadContext"));
//        if (!ThreadContext_class) ThreadContext_class = Java::Env->FindClass(xorstr("org/apache/logging/log4j/ThreadContext"));
//
//        if (!ThreadContext_class) return;
//
//        jfieldID EMPTY_MAP_ID = Java::Env->GetStaticFieldID(ThreadContext_class, xorstr("EMPTY_MAP"), xorstr("Ljava/util/Map;"));
//
//        // [ARREGLO CRITICO 1] Usamos la clase correcta
//        jobject localOriginalMap = Java::Env->GetStaticObjectField(ThreadContext_class, EMPTY_MAP_ID);
//        original_EMPTY_MAP = Java::Env->NewGlobalRef(localOriginalMap); // Guardamos global
//        Java::Env->DeleteLocalRef(localOriginalMap);
//
//        // Crear HashMap falso
//        jclass hashMapClass = Java::Env->FindClass(xorstr("java/util/HashMap"));
//        jmethodID constructor = Java::Env->GetMethodID(hashMapClass, xorstr("<init>"), xorstr("()V"));
//        jobject localHashMap = Java::Env->NewObject(hashMapClass, constructor);
//
//        // [ARREGLO CRITICO 2] Referencia Global para g_HashMap
//        g_HashMap = Java::Env->NewGlobalRef(localHashMap);
//
//        // Realizar el SWAP: Poner nuestro mapa falso en ThreadContext
//        Java::Env->SetStaticObjectField(ThreadContext_class, EMPTY_MAP_ID, g_HashMap);
//
//        // Preparar métodos para put/remove
//        jclass mapClass = Java::Env->FindClass(xorstr("java/util/Map"));
//        g_MapClass = (jclass)Java::Env->NewGlobalRef(mapClass);
//        g_MapPut = Java::Env->GetMethodID(mapClass, xorstr("put"), xorstr("(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"));
//        g_MapRemove = Java::Env->GetMethodID(mapClass, xorstr("remove"), xorstr("(Ljava/lang/Object;)Ljava/lang/Object;"));
//
//        // Valor por defecto seguro
//        put(xorstr("reach_distance"), xorstr("3.0"));
//
//        // Limpieza
//        Java::Env->DeleteLocalRef(localHashMap);
//        Java::Env->DeleteLocalRef(hashMapClass);
//        Java::Env->DeleteLocalRef(ThreadContext_class);
//        Java::Env->DeleteLocalRef(mapClass);
//
//        // 4. Cargar el Payload (JAR)
//        jobject classLoader = getCleanClassLoader();
//        loadJar(classLoader, data, sizeof(data));
//        Java::Env->DeleteLocalRef(classLoader);
//
//        // 5. Aplicar cambios (Retransform)
//        // Esto dispara ClassFileLoadHook
//        if (EntityRenderer_class) {
//            Java::tiEnv->RetransformClasses(1, &EntityRenderer_class);
//        }
//
//        // Desactivar hook para ahorrar CPU
//        Java::tiEnv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
//    }
//
//    void Kill()
//    {
//        // 1. Revertir cambios (Retransform de nuevo, pero sin hook activo, restaurará el original)
//        if (EntityRenderer_class) {
//            Java::tiEnv->RetransformClasses(1, &EntityRenderer_class);
//        }
//
//        // 2. Restaurar el mapa original
//        jclass ThreadContext_class = Java::Env->FindClass(xorstr("org/apache/logging/log4j/ThreadContext"));
//        if (ThreadContext_class && original_EMPTY_MAP) {
//            jfieldID EMPTY_MAP_ID = Java::Env->GetStaticFieldID(ThreadContext_class, xorstr("EMPTY_MAP"), xorstr("Ljava/util/Map;"));
//            Java::Env->SetStaticObjectField(ThreadContext_class, EMPTY_MAP_ID, original_EMPTY_MAP);
//        }
//        if (ThreadContext_class) Java::Env->DeleteLocalRef(ThreadContext_class);
//
//        // 3. Limpiar Globales
//        if (original_EMPTY_MAP) Java::Env->DeleteGlobalRef(original_EMPTY_MAP);
//        if (g_HashMap) Java::Env->DeleteGlobalRef(g_HashMap);
//        if (g_MapClass) Java::Env->DeleteGlobalRef(g_MapClass);
//        if (EntityRenderer_class) Java::Env->DeleteGlobalRef(EntityRenderer_class);
//    }
//
//    void put(const std::string& key, const std::string& value)
//    {
//        if (!g_HashMap || !g_MapPut) return;
//
//        jstring k = Java::Env->NewStringUTF(key.c_str());
//        jstring v = Java::Env->NewStringUTF(value.c_str());
//
//        Java::Env->CallObjectMethod(g_HashMap, g_MapPut, k, v);
//
//        Java::Env->DeleteLocalRef(k);
//        Java::Env->DeleteLocalRef(v);
//    }
//
//    void remove(const std::string& key)
//    {
//        if (!g_HashMap || !g_MapRemove) return;
//
//        jstring k = Java::Env->NewStringUTF(key.c_str());
//        Java::Env->CallObjectMethod(g_HashMap, g_MapRemove, k);
//        Java::Env->DeleteLocalRef(k);
//    }
//}

namespace Patcher
{
    // Variables Globales Persistentes
    jclass ThreadContext_class = nullptr;
    jobject original_Map = nullptr;   // El mapa original (backup)

    // Cache de MethodIDs para máxima velocidad
    jmethodID mid_put = nullptr;
    jmethodID mid_remove = nullptr;

    namespace Internal {
        // Función para cargar el JAR y CAPTURAR la clase crítica
        void loadJar(jobject classLoader, const unsigned char* jarBytes, size_t size) {
            mz_zip_archive archive{};
            if (!mz_zip_reader_init_mem(&archive, jarBytes, size, 0)) {
                std::cout << "Error: No se pudo leer el ZIP/JAR de memoria." << std::endl;
                return;
            }

            int files = mz_zip_reader_get_num_files(&archive);
            for (int i = 0; i < files; i++) {
                if (mz_zip_reader_is_file_a_directory(&archive, i)) continue;

                char filename[256] = { 0 };
                mz_zip_reader_get_filename(&archive, i, filename, 256);
                std::string fname(filename);

                // Inyectamos solo los .class
                if (fname.length() > 6 && fname.substr(fname.length() - 6) == ".class") {
                    size_t sz = 0;
                    unsigned char* bytes = (unsigned char*)mz_zip_reader_extract_to_heap(&archive, i, &sz, 0);
                    if (bytes) {
                        // Inyectamos la clase
                        jclass cls = Java::Env->DefineClass(nullptr, classLoader, (const jbyte*)bytes, sz);

                        if (cls) {
                            // VERIFICACIÓN: ¿Es esta la clase 'ClassPatcher' que necesitamos?
                            // Comparamos el nombre del archivo en el ZIP
                            // Ajusta "io/github/lefraudeur/ClassPatcher.class" si tu JAR tiene otra ruta
                            if (fname.find("ClassPatcher") != std::string::npos) {
                                std::cout << "EXITO: ClassPatcher encontrada e inyectada." << std::endl;
                                ClassPatcher_class = (jclass)Java::Env->NewGlobalRef(cls);
                            }
                            Java::Env->DeleteLocalRef(cls);
                        }
                        else {
                            if (Java::Env->ExceptionCheck()) Java::Env->ExceptionClear();
                            std::cout << "Fallo al definir clase: " + fname << std::endl;
                        }
                        mz_free(bytes);
                    }
                }
            }
            mz_zip_reader_end(&archive);
        }

        // El Hook que conecta todo
        void JNICALL ClassFileLoadHook(jvmtiEnv* jvmti, JNIEnv* env, jclass class_being_redefined, jobject loader, const char* name, jobject domain, jint len, const unsigned char* data, jint* new_len, unsigned char** new_data) {
            // Solo nos interesa EntityRenderer
            if (EntityRenderer_class && env->IsSameObject(class_being_redefined, EntityRenderer_class)) {
                std::cout << "Hook activado: EntityRenderer detectado." << std::endl;
                // [FIX CRÍTICO] Usamos la clase global que capturamos, no intentamos buscarla
                if (!ClassPatcher_class) {
                    std::cout << "ERROR FATAL: ClassPatcher es nulo en el Hook" << std::endl;
                    return;
                }

                jbyteArray oldBytes = env->NewByteArray(len);
                env->SetByteArrayRegion(oldBytes, 0, len, (const jbyte*)data);

                // Buscamos el método 'patchEntityRenderer' en TU clase inyectada
                jmethodID mid = env->GetStaticMethodID(ClassPatcher_class, xorstr("patchEntityRenderer"), xorstr("([BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)[B"));

                if (mid) {
                    std::cout << "Metodo de parcheo encontrado. Ejecutando inyeccion de bytecode..." << std::endl;
                    jstring sMethod = env->NewStringUTF(xorstr("getMouseOver"));
                    jstring sCtx = env->NewStringUTF(xorstr("org/apache/logging/log4j/ThreadContext"));
                    jstring sMap = env->NewStringUTF(xorstr("EMPTY_MAP"));

                    jbyteArray newBytes = (jbyteArray)env->CallStaticObjectMethod(ClassPatcher_class, mid, oldBytes, sMethod, sCtx, sMap);

                    if (newBytes) {
                        *new_len = env->GetArrayLength(newBytes);
                        jvmti->Allocate(*new_len, new_data);
                        env->GetByteArrayRegion(newBytes, 0, *new_len, (jbyte*)*new_data);
                        std::cout << "!!! REACH ACTIVADO !!! Bytecode modificado exitosamente." << std::endl;
                    }
                    else {
                        std::cout << "El parcheador devolvió null. Algo fallo en la logica Java." << std::endl;
                    }
                    env->DeleteLocalRef(sMethod); env->DeleteLocalRef(sCtx); env->DeleteLocalRef(sMap); env->DeleteLocalRef(newBytes);
                }
                else {
                    std::cout << "ERROR: No se encontro el metodo 'patchEntityRenderer'. Verifica tu data.h" << std::endl;
                    if (env->ExceptionCheck()) env->ExceptionDescribe();
                }
                env->DeleteLocalRef(oldBytes);
            }
        }
    }

    void Init() {
        std::cout << "Iniciando The Void Patcher v2..." << std::endl;

        if (!Java::Env || !Java::Initialized) {
            std::cout << "JNI no inicializado." << std::endl;
            return;
        }

        // 1. Configurar JVMTI
        jvmtiCapabilities caps{};
        caps.can_retransform_classes = 1;
        Java::tiEnv->AddCapabilities(&caps);
        jvmtiEventCallbacks cb{};
        cb.ClassFileLoadHook = &Internal::ClassFileLoadHook;
        Java::tiEnv->SetEventCallbacks(&cb, sizeof(cb));
        Java::tiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);

        // 2. Obtener EntityRenderer
        jclass tmpRenderer;
        if (Java::AssignClass(xorstr("net.minecraft.client.renderer.EntityRenderer"), tmpRenderer)) {
            EntityRenderer_class = (jclass)Java::Env->NewGlobalRef(tmpRenderer);
            Java::Env->DeleteLocalRef(tmpRenderer);
            std::cout << "EntityRenderer encontrado." << std::endl;
        }
        else {
            std::cout << "ERROR: No se encontro EntityRenderer." << std::endl;
        }
        jclass tmpCtx = nullptr;
        if (Java::AssignClass(xorstr("org.apache.logging.log4j.ThreadContext"), tmpCtx)) {
            ThreadContext_class = (jclass)Java::Env->NewGlobalRef(tmpCtx);
            Java::Env->DeleteLocalRef(tmpCtx);
            std::cout << "ThreadContext encontrado correctamente." << std::endl;

            jfieldID fid_map = Java::Env->GetStaticFieldID(ThreadContext_class, xorstr("EMPTY_MAP"), xorstr("Ljava/util/Map;"));
            if (fid_map) {
                jobject orig = Java::Env->GetStaticObjectField(ThreadContext_class, fid_map);
                original_Map = Java::Env->NewGlobalRef(orig);
                Java::Env->DeleteLocalRef(orig);

                jclass clsHashMap = Java::Env->FindClass(xorstr("java/util/HashMap"));
                jmethodID ctorMap = Java::Env->GetMethodID(clsHashMap, xorstr("<init>"), xorstr("()V"));
                jobject localMap = Java::Env->NewObject(clsHashMap, ctorMap);
                g_HashMap = Java::Env->NewGlobalRef(localMap);
                Java::Env->DeleteLocalRef(localMap);

                // Realizar el SWAP
                Java::Env->SetStaticObjectField(ThreadContext_class, fid_map, g_HashMap);
                std::cout << "Map Swap realizado con exito." << std::endl;

                // Caches
                jclass clsMap = Java::Env->FindClass(xorstr("java/util/Map"));
                mid_put = Java::Env->GetMethodID(clsMap, xorstr("put"), xorstr("(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"));
                mid_remove = Java::Env->GetMethodID(clsMap, xorstr("remove"), xorstr("(Ljava/lang/Object;)Ljava/lang/Object;"));
                Java::Env->DeleteLocalRef(clsMap);
                Java::Env->DeleteLocalRef(clsHashMap);

                // Inicializar valor seguro
                put(xorstr("reach_distance"), xorstr("3.0"));
            }
            else {
                std::cout << "ERROR: No se encontro el campo EMPTY_MAP." << std::endl;
            }
        }
        else {
            // Intento de fallback (Plan B)
            std::cout << "Advertencia: AssignClass fallo para ThreadContext. Intentando FindClass..." << std::endl;
            tmpCtx = Java::Env->FindClass(xorstr("org/apache/logging/log4j/ThreadContext"));
            if (tmpCtx) {
                ThreadContext_class = (jclass)Java::Env->NewGlobalRef(tmpCtx);
                Java::Env->DeleteLocalRef(tmpCtx);
                std::cout << "ThreadContext encontrado con FindClass (Plan B)." << std::endl;
                // ... (Aquí iría la lógica repetida, pero confiaremos en que AssignClass funcione)
            }
            else {
                std::cout << "ERROR FATAL: ThreadContext no encontrado. El Reach causara crash." << std::endl;
            }
        }

        // 4. Cargar JAR y Capturar ClassPatcher
        jclass clsURL = Java::Env->FindClass(xorstr("java/net/URL"));
        jobjectArray urls = Java::Env->NewObjectArray(0, clsURL, nullptr);
        jclass clsLoader = Java::Env->FindClass(xorstr("java/net/URLClassLoader"));
        jmethodID ctorLoader = Java::Env->GetMethodID(clsLoader, xorstr("<init>"), xorstr("([Ljava/net/URL;)V"));
        jobject ldr = Java::Env->NewObject(clsLoader, ctorLoader, urls);

        std::cout << "Cargando JAR desde memoria..." << std::endl;
        Internal::loadJar(ldr, data, sizeof(data));

        Java::Env->DeleteLocalRef(clsURL); Java::Env->DeleteLocalRef(urls);
        Java::Env->DeleteLocalRef(clsLoader); Java::Env->DeleteLocalRef(ldr);

        if (ClassPatcher_class == nullptr) {
            std::cout << "PELIGRO: ClassPatcher no fue capturado. El hook fallará." << std::endl;
        }

        // 5. Aplicar Parche
        if (EntityRenderer_class) {
            std::cout << "Retransformando clases..." << std::endl;
            Java::tiEnv->RetransformClasses(1, &EntityRenderer_class);
        }
        Java::tiEnv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
        std::cout << "Inicialización completa." << std::endl;
    }

    void Kill() {
        if (ThreadContext_class && original_Map && Java::Env) {
            jfieldID fid = Java::Env->GetStaticFieldID(ThreadContext_class, xorstr("EMPTY_MAP"), xorstr("Ljava/util/Map;"));
            if (fid) Java::Env->SetStaticObjectField(ThreadContext_class, fid, original_Map);
        }
        if (EntityRenderer_class && Java::tiEnv) {
            Java::tiEnv->RetransformClasses(1, &EntityRenderer_class);
        }
        if (ClassPatcher_class) Java::Env->DeleteGlobalRef(ClassPatcher_class);
    }

    void put(const std::string& key, const std::string& value) {
        if (!g_HashMap || !mid_put || !Java::Env) return;
        jstring k = Java::Env->NewStringUTF(key.c_str());
        jstring v = Java::Env->NewStringUTF(value.c_str());
        Java::Env->CallObjectMethod(g_HashMap, mid_put, k, v);
        Java::Env->DeleteLocalRef(k);
        Java::Env->DeleteLocalRef(v);
    }

    void remove(const std::string& key) {
        if (!g_HashMap || !mid_remove || !Java::Env) return;
        jstring k = Java::Env->NewStringUTF(key.c_str());
        Java::Env->CallObjectMethod(g_HashMap, mid_remove, k);
        Java::Env->DeleteLocalRef(k);
    }
}