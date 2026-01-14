#include "main.h"

#ifdef _DEBUG
#define DEBUG_LOG(x) std::cout << "[THE VOID] " << x << std::endl
#else
#define DEBUG_LOG(x)
#endif


namespace TheVoid {
	namespace Main {

		void CMain::Init(HMODULE hModule) {
#ifdef _DEBUG
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            DEBUG_LOG("Inyeccion exitosa. La oscuridad ha llegado.");
#endif

            // 1. INICIALIZAR LA JVM (CRÍTICO)
            DEBUG_LOG("Buscando JVM...");
            if (!TheVoid::Java::Init()) {
                DEBUG_LOG("ERROR FATAL: No se encontro jvm.dll o JNI_GetCreatedJavaVMs fallo.");
                // Si falla esto, no tiene sentido seguir
            }
            else {
                DEBUG_LOG("JVM Encontrada exitosamente.");
            }

            // 2. Inicializar el Mapper
            //TheVoid::Java::Mapper::Init();

            DEBUG_LOG("Mapper inicializado.");

            DEBUG_LOG("Esperando a que el juego cargue...");
            
            bool mapperInitialized = false;
            // Bucle principal
            while (!GetAsyncKeyState(VK_F4)) {

                if (!TheVoid::Java::EnsureAttached()) {
                    static bool printedError = false;
                    if (!printedError) {
                        DEBUG_LOG("ERROR: Fallo al adjuntar hilo a la JVM (EnsureAttached false)");
                        printedError = true;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                // 3. AHORA SÍ: Inicializamos el Mapper (Solo una vez)
                if (!mapperInitialized) {
                    std::cout << "[THE VOID] JVM Conectada. Inicializando Mapper..." << std::endl;

                    // Verificación de seguridad extrema antes de llamar
                    if (TheVoid::Java::env == nullptr) {
                        std::cout << "[FATAL] Java::env sigue siendo NULL. Pánico." << std::endl;
                        continue;
                    }

                    // Aquí es seguro llamar a Init porque 'env' ya es válido
                    TheVoid::Java::Mapper::Init();

                    mapperInitialized = true; // Marcamos como listo para no repetirlo
                    std::cout << "[THE VOID] Cliente Listo para el combate." << std::endl;
                }

                CMain::Test();
                TheVoid::Modules::LeftAutoClicker::Instance().Update();


                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

#ifdef _DEBUG
            if (f) fclose(f);
            FreeConsole();
#endif
            FreeLibraryAndExitThread(hModule, 0);

		}
	}
}