#include "patcher.h"
#include "miniz.h"
#include "data.h"
#include <functional>
#include "../../../ext/xorstr/xorstr.h"
#include <iostream>

namespace Patcher
{
	namespace
	{
		void loadJar(jobject classLoader, const unsigned char* jarBytes, size_t size)
		{
			mz_zip_archive archive{};
			if (!mz_zip_reader_init_mem(&archive, jarBytes, size, 0))
			{
				return;
			}
			mz_uint file_number = mz_zip_reader_get_num_files(&archive);
			for (mz_uint i = 0; i < file_number; i++)
			{

				if (!mz_zip_reader_is_file_supported(&archive, i) || mz_zip_reader_is_file_a_directory(&archive, i)) 
					continue;

				char str[256] = { 0 };
				mz_zip_reader_get_filename(&archive, i, str, 256);
				std::string filename(str);

				if (filename.substr(filename.size() - 6) != ".class")
					continue;

				size_t classBytes_size = 0;
				unsigned char* classBytes = (unsigned char*)mz_zip_reader_extract_to_heap(&archive, i, &classBytes_size, 0);
				if (!classBytes)
				{
					mz_zip_reader_end(&archive);
					return;
				}

				jclass jaclass = Java::Env->DefineClass(nullptr, classLoader, (const jbyte*)classBytes, classBytes_size);
				if(jaclass)Java::Env->DeleteLocalRef(jaclass);
				mz_free(classBytes);
			}
			mz_zip_reader_end(&archive);
			return;
		}

		void gc()
		{
			jclass System_class = Java::Env->FindClass("java/lang/System");
			jmethodID gcID = Java::Env->GetStaticMethodID(System_class, "gc", "()V");
			Java::Env->CallStaticVoidMethod(System_class, gcID);
			Java::Env->DeleteLocalRef(System_class);
		}

		jobject newClassLoader()
		{
			jclass urlClass = Java::Env->FindClass("java/net/URL");
			jmethodID urlContructor = Java::Env->GetMethodID(urlClass, "<init>", "(Ljava/lang/String;)V");
			jstring str = Java::Env->NewStringUTF("file://ftp.yoyodyne.com/pub/files/foobar.txt");
			jobject url = Java::Env->NewObject(urlClass, urlContructor, str);
			jobjectArray urls = Java::Env->NewObjectArray(1, urlClass, url);
			jclass URLClassLoaderClass = Java::Env->FindClass("java/net/URLClassLoader");
			jmethodID URLClassLoaderContructor = Java::Env->GetMethodID(URLClassLoaderClass, "<init>", "([Ljava/net/URL;)V");
			jobject URLClassLoader = Java::Env->NewObject(URLClassLoaderClass, URLClassLoaderContructor, urls);

			Java::Env->DeleteLocalRef(urlClass);
			Java::Env->DeleteLocalRef(url);
			Java::Env->DeleteLocalRef(str);
			Java::Env->DeleteLocalRef(urls);
			Java::Env->DeleteLocalRef(URLClassLoaderClass);

			return URLClassLoader;
		}

		void retransformClasses()
		{
			Java::tiEnv->RetransformClasses(1, &EntityRenderer_class);
		}

		void JNICALL ClassFileLoadHook
		(
			jvmtiEnv* jvmti_env, 
			JNIEnv* jni_env, 
			jclass class_being_redefined, 
			jobject loader, 
			const char* name, 
			jobject protection_domain, 
			jint class_data_len, 
			const unsigned char* class_data, 
			jint* new_class_data_len, 
			unsigned char** new_class_data
		)
		{
			std::function<void(const std::string&, const std::string&)> patchClass = [=](const std::string& patchMethod, const std::string& methodToPatch)
			{
				jclass ClassPatcherClass = Java::findClass(jni_env, jvmti_env, "io/github/lefraudeur/ClassPatcher");
				jbyteArray original_class_bytes = jni_env->NewByteArray(class_data_len);
				jni_env->SetByteArrayRegion(original_class_bytes, 0, class_data_len, (const jbyte*)class_data);

				jmethodID patchMethodID = jni_env->GetStaticMethodID(ClassPatcherClass, patchMethod.c_str(), "([BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)[B");
				jstring methodToPatchStr = jni_env->NewStringUTF(methodToPatch.c_str());
				jstring ThreadContextClassName = jni_env->NewStringUTF("org/apache/logging/log4j/ThreadContext");
				jstring emptyMapName = jni_env->NewStringUTF("EMPTY_MAP");

				jbyteArray new_class_bytes = (jbyteArray)jni_env->CallStaticObjectMethod(
					ClassPatcherClass,
					patchMethodID,
					original_class_bytes,
					methodToPatchStr,
					ThreadContextClassName,
					emptyMapName
				);

				jni_env->DeleteLocalRef(ClassPatcherClass);
				jni_env->DeleteLocalRef(methodToPatchStr);
				jni_env->DeleteLocalRef(ThreadContextClassName);
				jni_env->DeleteLocalRef(emptyMapName);

				jni_env->DeleteLocalRef(original_class_bytes);
				*new_class_data_len = jni_env->GetArrayLength(new_class_bytes);
				jvmti_env->Allocate(*new_class_data_len, new_class_data);
				jni_env->GetByteArrayRegion(new_class_bytes, 0, *new_class_data_len, (jbyte*)*new_class_data);
				jni_env->DeleteLocalRef(new_class_bytes);
			};

			if (jni_env->IsSameObject(class_being_redefined, EntityRenderer_class))
			{
				patchClass("patchEntityRenderer", "getMouseOver");
			}
		}
	}

	jobject getCleanClassLoader()
	{
		// Encriptamos las cadenas de Java Reflection
		// En el binario, "java/net/URL" será basura ilegible hasta que se ejecute esta línea.
		jclass urlClass = Java::Env->FindClass(xorstr("java/net/URL"));
		jobjectArray emptyUrls = Java::Env->NewObjectArray(0, urlClass, nullptr);

		jclass URLClassLoaderClass = Java::Env->FindClass(xorstr("java/net/URLClassLoader"));
		jmethodID ctor = Java::Env->GetMethodID(URLClassLoaderClass, xorstr("<init>"), xorstr("([Ljava/net/URL;)V"));

		jobject classLoader = Java::Env->NewObject(URLClassLoaderClass, ctor, emptyUrls);

		Java::Env->DeleteLocalRef(urlClass);
		Java::Env->DeleteLocalRef(emptyUrls);
		Java::Env->DeleteLocalRef(URLClassLoaderClass);

		return classLoader;
	}

	void Init()
	{
		// 1. Configuración de JVMTI (Estándar)
		jvmtiCapabilities capabilities{};
		capabilities.can_retransform_classes = JVMTI_ENABLE;
		Java::tiEnv->AddCapabilities(&capabilities);
		jvmtiEventCallbacks callbacks{};
		callbacks.ClassFileLoadHook = &ClassFileLoadHook;
		Java::tiEnv->SetEventCallbacks(&callbacks, sizeof(jvmtiEventCallbacks));
		Java::tiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);

		// 2. Obtener Clases Críticas
		Java::AssignClass(xorstr("net.minecraft.client.renderer.EntityRenderer"), EntityRenderer_class);

		// Usamos el helper findClass para buscar ThreadContext en todos los loaders
		jclass ThreadContext_class = Java::findClass(Java::Env, Java::tiEnv, xorstr("org/apache/logging/log4j/ThreadContext"));

		// ARREGLO 1: Validación estricta
		if (!ThreadContext_class) {
			// Si no lo encontramos, intentamos buscarlo manualmente por nombre JNI
			ThreadContext_class = Java::Env->FindClass(xorstr("org/apache/logging/log4j/ThreadContext"));
		}

		if (!ThreadContext_class) return; // Si falla, abortamos para no crashear

		jfieldID EMPTY_MAP_ID = Java::Env->GetStaticFieldID(ThreadContext_class, xorstr("EMPTY_MAP"), xorstr("Ljava/util/Map;"));

		// ARREGLO 2: Corrección del error de JNI del código original.
		// ANTES: GetStaticObjectField(EntityRenderer_class, ...) -> MAL
		// AHORA: GetStaticObjectField(ThreadContext_class, ...) -> BIEN
		original_EMPTY_MAP = Java::Env->GetStaticObjectField(ThreadContext_class, EMPTY_MAP_ID);

		// 3. Crear el HashMap de reemplazo
		jclass hashMapClass = Java::Env->FindClass(xorstr("java/util/HashMap"));
		jmethodID constructor = Java::Env->GetMethodID(hashMapClass, xorstr("<init>"), xorstr("()V"));

		jobject localHashMap = Java::Env->NewObject(hashMapClass, constructor);
		g_HashMap = Java::Env->NewGlobalRef(localHashMap);

		// 4. EL INTERCAMBIO (The Swap)
		Java::Env->SetStaticObjectField(ThreadContext_class, EMPTY_MAP_ID, g_HashMap);

		// 5. Inicializar el mapa con valores por defecto (ARREGLO PARA NULL POINTER)
		// Esto evita el crash si el Java inyectado lee antes de que tú escribas algo.
		jclass mapClass = Java::Env->FindClass(xorstr("java/util/Map"));
		g_MapClass = (jclass)Java::Env->NewGlobalRef(mapClass);
		g_MapPut = Java::Env->GetMethodID(mapClass, xorstr("put"), xorstr("(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"));
		g_MapRemove = Java::Env->GetMethodID(mapClass, xorstr("remove"), xorstr("(Ljava/lang/Object;)Ljava/lang/Object;"));

		// Escribimos un valor por defecto seguro inmediatamente
		put(xorstr("reach_distance"), xorstr("3.0"));

		// Limpieza
		Java::Env->DeleteLocalRef(localHashMap);
		Java::Env->DeleteLocalRef(hashMapClass);
		Java::Env->DeleteLocalRef(ThreadContext_class);
		Java::Env->DeleteLocalRef(mapClass);

		// 6. Cargar el Payload
		jobject classLoader = getCleanClassLoader();
		loadJar(classLoader, data, sizeof(data));

		// Aplicar los cambios
		retransformClasses();
		Java::tiEnv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);

		Java::Env->DeleteLocalRef(classLoader);
	}

	void Kill()
	{
		// 1. Revertir cambios en la clase
		retransformClasses();

		// 2. Restaurar el mapa original
		// Ciframos las cadenas para no ser detectados al descargar la DLL
		jclass ThreadContext_class = Java::findClass(Java::Env, Java::tiEnv, xorstr("org/apache/logging/log4j/ThreadContext"));
		jfieldID EMPTY_MAP_ID = Java::Env->GetStaticFieldID(ThreadContext_class, xorstr("EMPTY_MAP"), xorstr("Ljava/util/Map;"));

		// Restauramos el objeto original que guardamos en Init
		Java::Env->SetStaticObjectField(ThreadContext_class, EMPTY_MAP_ID, original_EMPTY_MAP);

		// 3. Limpieza de memoria
		Java::Env->DeleteLocalRef(ThreadContext_class);

		// Liberamos nuestras referencias globales
		if (original_EMPTY_MAP) Java::Env->DeleteGlobalRef(original_EMPTY_MAP); // Importante: DeleteGlobalRef, no Local
		if (g_HashMap) Java::Env->DeleteGlobalRef(g_HashMap);         // Borramos nuestra referencia al mapa fake
		if (g_MapClass) Java::Env->DeleteGlobalRef(g_MapClass);

		// Limpiamos la clase EntityRenderer si la guardamos como Global
		if (EntityRenderer_class) Java::Env->DeleteGlobalRef(EntityRenderer_class);
	}

	void put(const std::string& key, const std::string& value)
	{
		// Seguridad: Usamos g_HashMap ahora
		if (!g_HashMap || !g_MapPut) return;

		jstring k = Java::Env->NewStringUTF(key.c_str());
		jstring v = Java::Env->NewStringUTF(value.c_str());

		Java::Env->CallObjectMethod(g_HashMap, g_MapPut, k, v);

		Java::Env->DeleteLocalRef(k);
		Java::Env->DeleteLocalRef(v);
	}

	void remove(const std::string& key)
	{
		if (!g_HashMap || !g_MapRemove) return;

		jstring k = Java::Env->NewStringUTF(key.c_str());
		Java::Env->CallObjectMethod(g_HashMap, g_MapRemove, k);
		Java::Env->DeleteLocalRef(k);
	}
}
