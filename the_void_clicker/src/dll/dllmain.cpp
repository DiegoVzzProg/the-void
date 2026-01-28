#include "../main/main.h"

DWORD WINAPI MainThread(LPVOID lpParam) {
    HMODULE hModule = static_cast<HMODULE>(lpParam);
    Main::CMain::Get().Init(hModule);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        HANDLE hThread = CreateThread(
            nullptr,
            0,
            MainThread,
            hModule,
            0,
            nullptr
        );

        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}

