#pragma once
#include <jni.h>
#include <thread>
#include <chrono>
#include <windows.h>
#include <iostream>

#include "./encrypt/encrypt.h"

typedef jint(JNICALL* tJNI_GetCreatedJavaVMs)(JavaVM**, jsize, jsize*);

namespace Java {
	class CEnv {
	public:
		inline static JNIEnv* env = nullptr;
		inline static HWND GameWindow = nullptr;
		inline static JavaVM* vm = nullptr;
		inline static bool initialized = false;
		inline static jobject ClassLoaderObj = nullptr;

        static bool Init();

        static bool EnsureAttached();

		static void FixClassLoader();
	};
}