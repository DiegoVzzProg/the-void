#include "../../java/java.h"
#include "VersionDetector.h"
#include <algorithm>
#include <wtypes.h>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <filesystem>
#include <string>

struct EnumData {
    DWORD processId;
    HWND hWnd;
};

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
    EnumData& data = *(EnumData*)lParam;
    DWORD pid = 0;

    GetWindowThreadProcessId(handle, &pid);

    if (pid == data.processId) {
      
        if (handle == GetConsoleWindow()) {
            return TRUE;
        }

        if (IsWindowVisible(handle)) {

            char className[256];
            GetClassNameA(handle, className, sizeof(className));

            if (strstr(className, "LWJGL") || strstr(className, "GLFW") || IsWindowVisible(handle)) {
                data.hWnd = handle;
                return FALSE;
            }
        }
    }
    return TRUE;
}

HWND GetCurrentGameWindow() {
    EnumData data;
    data.processId = GetCurrentProcessId();
    data.hWnd = 0;

    EnumWindows(EnumWindowsCallback, (LPARAM)&data);

    return data.hWnd;
}


std::string VersionDetector::GetClientName() {
    char title[256];

    HWND hwnd = GetCurrentGameWindow();

    if (!hwnd) {
        std::cout << "[Error] No se encontró la ventana del juego por PID." << std::endl;
        return "Unknown";
    }

    GetWindowTextA(hwnd, title, sizeof(title));

    std::string titleStr(title);

    std::cout << "[The Void] Ventana Analizada: " << titleStr << std::endl;

    std::transform(titleStr.begin(), titleStr.end(), titleStr.begin(), ::tolower);

    if (titleStr.find("lunar") != std::string::npos) return "Lunar";
    if (titleStr.find("badlion") != std::string::npos) return "Badlion";
    if (titleStr.find("feather") != std::string::npos) return "Feather";
    if (titleStr.find("forge") != std::string::npos) return "Forge";
    if (titleStr.find("cheatbreaker") != std::string::npos) return "CheatBreaker";

    return "Vanilla";
}

bool ClassExists(const char* className) {
    jclass cls = Java::Env->FindClass(className);
    if (cls) {
        Java::Env->DeleteLocalRef(cls);
        return true;
    }

    // CRÍTICO: Si FindClass falla, deja una excepción pendiente en la JVM.
    // Debemos limpiarla o la siguiente llamada JNI matará el juego.
    Java::Env->ExceptionClear();
    return false;
}



GameVersions VersionDetector::GetVersion() {
    std::string client = GetClientName();
    bool isForge = ClassExists("net/minecraftforge/common/MinecraftForge");

    bool is1_8_9 = ClassExists("ave");

    bool is1_7_10 = ClassExists("bao");

    if (client == "Lunar") {
        if (is1_8_9) return LUNAR_1_8;
        if (is1_7_10) return LUNAR_1_7_10;
        return LUNAR_1_8;
    }

    if (isForge) {
        if (is1_8_9) return FORGE_1_8;
        if (is1_7_10) return FORGE_1_7_10;
    }

    if (is1_8_9) return CASUAL_1_8;
    if (is1_7_10) return CASUAL_1_7_10;

    return CASUAL_1_8;
}