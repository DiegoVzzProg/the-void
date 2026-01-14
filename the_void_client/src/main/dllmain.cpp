// -------------------------
// [[[[[[[ THE VOID ]]]]]]]]
// the ultimate ghost client
// -------------------------

#include "main.h"


void MainThread(HMODULE hModule) {
    TheVoid::Main::CMain::Init(hModule);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        HANDLE hThread = CreateThread(
            nullptr,
            0,
            (LPTHREAD_START_ROUTINE)MainThread,
            hModule,
            0,
            nullptr
        );

        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}

