#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <filesystem>
#include <string>

const std::wstring TARGET_PROCESS = L"javaw.exe";
const std::string DLL_NAME = "the_void_client/the_void_client.dll";//"the_void_internal.dll";

DWORD GetProcessIdByName(const std::wstring& processName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(snapshot, &pe32)) {
            do {
                if (processName == pe32.szExeFile) {
                    pid = pe32.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &pe32));
        }
        CloseHandle(snapshot);
    }
    return pid;
}

int main()
{
    std::cout << "[INIT] Iniciando inyector de 'The Void'..." << std::endl;

    // 1. Verificar que la DLL existe en el directorio actual
    if (!std::filesystem::exists(DLL_NAME)) {
        std::cerr << "[ERROR] No se encuentra '" << DLL_NAME << "'. Asegurate de compilar la DLL y ponerla junto al inyector." << std::endl;
        system("pause");
        return 1;
    }

    // Obtener ruta absoluta de la DLL (Requisito para LoadLibrary) [cite: 89]
    std::string dllPath = std::filesystem::absolute(DLL_NAME).string();
    std::cout << "[INFO] DLL encontrada en: " << dllPath << std::endl;

    // 2. Buscar el proceso Minecraft (javaw.exe) [cite: 90]
    std::wcout << L"[INFO] Buscando proceso: " << TARGET_PROCESS << L"..." << std::endl;
    DWORD pid = GetProcessIdByName(TARGET_PROCESS);

    if (pid == 0) {
        std::cerr << "[ERROR] Minecraft no esta en ejecucion." << std::endl;
        system("pause");
        return 1;
    }
    std::cout << "[EXITO] PID encontrado: " << pid << std::endl;

    // 3. Abrir el proceso (Adquisición de Handle) [cite: 51, 96]
    // Usamos PROCESS_ALL_ACCESS para desarrollo, pero en prod seríamos más sutiles.
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        std::cerr << "[ERROR] Fallo al abrir el proceso. ¿Ejecuta como Admin?" << std::endl;
        return 1;
    }

    // 4. Asignación de Memoria (VirtualAllocEx) [cite: 53, 101]
    // Reservamos espacio dentro de javaw.exe para escribir la ruta de nuestra DLL
    LPVOID pRemotePath = VirtualAllocEx(hProcess, nullptr, dllPath.length() + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!pRemotePath) {
        std::cerr << "[ERROR] VirtualAllocEx fallo." << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    // 5. Escritura de la Carga (WriteProcessMemory) [cite: 55, 109]
    // Copiamos la cadena de texto de la ruta en la memoria reservada
    if (!WriteProcessMemory(hProcess, pRemotePath, dllPath.c_str(), dllPath.length() + 1, nullptr)) {
        std::cerr << "[ERROR] WriteProcessMemory fallo." << std::endl;
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 6. Ejecución Remota (CreateRemoteThread) [cite: 56, 115]
    // Buscamos LoadLibraryA en kernel32.dll y creamos un hilo en Minecraft que la ejecute
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, pLoadLibrary, pRemotePath, 0, nullptr);
    if (!hThread) {
        std::cerr << "[ERROR] CreateRemoteThread fallo. Posible bloqueo de Anti-Cheat." << std::endl;
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 7. Limpieza [cite: 129]
    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE); // Liberamos la memoria de la ruta, ya no se necesita
    CloseHandle(hThread);
    CloseHandle(hProcess);

    std::cout << "[EXITO] Inyeccion completada. 'The Void' esta activo." << std::endl;
    system("pause");
}