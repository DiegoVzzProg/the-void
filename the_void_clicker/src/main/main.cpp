#include "main.h"

namespace Main {
	void CMain::Init(HMODULE hModule) {

#ifdef _DEBUG
		AllocConsole();
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
#endif

		DEBUG_LOG("Inyectando...");
		DEBUG_LOG("Buscando JVM...");

		if (!Java::CEnv::Init()) {
			DEBUG_LOG("ERROR FATAL: No se encontro jvm.dll o JNI_GetCreatedJavaVMs fallo.");
		}

		DEBUG_LOG("Conexion a base de datos en cusro...");

		Online::CWeb::Init();

		DEBUG_LOG("Esperando a que el juego cargue...");

		while (!panic) {

			if (Online::CWeb::ModuleActivated("panic")) {
				panic = Online::CWeb::GetModuleValue("action") == "enable";
				continue;
			}

			if (!Java::CEnv::EnsureAttached()) {
				static bool printedError = false;
				if (!printedError) {
					DEBUG_LOG("ERROR: Fallo al adjuntar hilo a la JVM (EnsureAttached false)");
					printedError = true;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			static bool printedMapperInitialized = false;
			if (!printedMapperInitialized) {
				DEBUG_LOG("JVM Conectada. Inicializando Mapper...");

				Java::CEnv::FixClassLoader();

				DEBUG_LOG("Mapper Conectado...");
				printedMapperInitialized = true;
			}



			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

#ifdef _DEBUG
		if (f) fclose(f);
		FreeConsole();
#endif
		Online::CWeb::CleanUp();
		FreeLibraryAndExitThread(hModule, 0);
	}
}