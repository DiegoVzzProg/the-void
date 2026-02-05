#include "main.h"

namespace Main {
	void CMain::Init(HMODULE hModule) {

#ifdef _DEBUG
		AllocConsole();
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
#endif

		LOG_INFO(xorstr("Inyectando..."));
		LOG_INFO(xorstr("Buscando JVM..."));

		if (!Java::CEnv::Get().Init()) {
			LOG_ERROR(xorstr("ERROR FATAL: No se encontro jvm.dll o JNI_GetCreatedJavaVMs fallo."));
		}

		LOG_INFO(xorstr("Conexion a base de datos en cusro..."));

		Online::CWeb::Get().Init();

		LOG_INFO(xorstr("Esperando a que el juego cargue..."));

		while (!panic) {
			std::string modName = xorstr("panic");

			if (Online::CWeb::Get().IsModuleActive(modName)) {
				panic = true;
				continue;
			}

			if (!Java::CEnv::Get().EnsureAttached()) {
				static bool printedError = false;
				if (!printedError) {
					LOG_ERROR(xorstr("ERROR: Fallo al adjuntar hilo a la JVM (EnsureAttached false)"));
					printedError = true;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			static bool printedMapperInitialized = false;
			if (!printedMapperInitialized) {
				LOG_INFO(xorstr("JVM Conectada. Inicializando Mapper..."));

				Java::CEnv::Get().FixClassLoader();
				Java::CMapper::Get().Init();

				if (!Java::CMinecraft::Get().Initialize()) {
					LOG_WARN(xorstr("Esperando instancia de Minecraft..."));
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					continue;
				}
				// ----------------------

				LOG_INFO(xorstr("Mapper y Minecraft Conectados..."));
				printedMapperInitialized = true;
			}

			Feature::CModules::Get().Activated();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		LOG_INFO(xorstr("Cerrando servicios..."));

		Online::CWeb::Get().CleanUp();
		Java::CMapper::Get().Cleanup();
		Java::CEnv::Get().Cleanup();

		LOG_INFO(xorstr("Todo limpio. Adios!"));

#ifdef _DEBUG
		FreeConsole();
#endif
		FreeLibraryAndExitThread(hModule, 0);
	}
}