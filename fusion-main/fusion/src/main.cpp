#include "main.h"
#include <windows.h>
#include <iostream>
#include <cstdio>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Bcrypt.lib")
// A veces necesarias dependiendo de la versión de Windows SDK:
#pragma comment(lib, "Shlwapi.lib")

// 1. Función para asignar la consola (OpSec: Eliminar en versión Release final)
void CreateDebugConsole()
{
    if (!AllocConsole()) return;

    // Redireccionar flujos estándar a la nueva consola
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);

    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

    SetConsoleTitleA("[THE VOID] Debug Console - Do not close manually!");
    std::cout << "[SYSTEM] Consola asignada correctamente." << std::endl;
}

// 2. Wrapper del Hilo Principal
// Este será el nuevo punto de entrada del hilo. Configura el entorno y luego llama a tu Main.
DWORD WINAPI MainThreadWrapper(LPVOID lpParam)
{
    // A. Abrir consola para ver errores
    CreateDebugConsole();

    std::cout << "[INIT] Llamando a Main::Init()..." << std::endl;

    // B. Ejecutar la lógica del cliente
    // (Asegúrate de poner logs dentro de Main::Init y Base::Init también)
    Main::Init();

    // Nota: Si Main::Init es un bucle infinito, nunca llegaremos aquí hasta que se cierre.
    // Si Main::Init solo inicializa y retorna, el hilo morirá aquí (cuidado con eso).
    // Asumiremos que Main::Init maneja el ciclo de vida o retorna tras cargar los hooks.

    std::cout << "[EXIT] Hilo principal finalizado." << std::endl;
    return 0;
}

void Main::Init()
{
    // Aquí dentro deberías poner std::cout << "Entrando a Base::Init" << endl;
    Base::Init();
}

void Main::Kill()
{
    Base::Kill();

    // Liberar consola al descargar la DLL
    FreeConsole();
    FreeLibraryAndExitThread(Main::HModule, 0);
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        Main::HModule = hModule;
        DisableThreadLibraryCalls(hModule);

        // CAMBIO CRITICO:
        // En lugar de llamar a Main::Init directamente, llamamos a nuestro Wrapper.
        // Esto garantiza que la consola exista ANTES de que cualquier código del cheat se ejecute.
        HANDLE hThread = CreateThread(nullptr, 0, MainThreadWrapper, hModule, 0, nullptr);

        if (hThread) CloseHandle(hThread);
    }

    return TRUE;
}