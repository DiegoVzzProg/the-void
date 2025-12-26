#pragma once
#include "../java/java.h"

namespace Patcher
{
	namespace
	{
		inline jobject original_EMPTY_MAP = nullptr;
		inline jobject EMPTY_MAP = nullptr;
		inline jclass EntityRenderer_class = nullptr;

		inline static jclass g_MapClass = nullptr;
		inline static jmethodID g_MapPut = nullptr;
		inline static jmethodID g_MapRemove = nullptr;
		inline static jobject g_ConcurrentMap = nullptr;
		inline static jobject g_HashMap = nullptr;

		void loadJar(jobject classLoader, const unsigned char* jarBytes, size_t size);
		void gc();
		jobject newClassLoader();
		void retransformClasses();
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
		);
	}
	void Init();
	void Kill();
	void put(const std::string& key, const std::string& value);
	void remove(const std::string& key);
}