#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>

// Incluye tu encriptador
#include "../data/encrypt/encrypt.h" 

// Si estamos en Release, definimos NDEBUG para que no compile los logs
#ifdef NDEBUG
#define LOG_INFO(msg, ...)
#define LOG_WARN(msg, ...)
#define LOG_ERROR(msg, ...)
#else
// Macros para facilitar el uso (detectan automáticamente el nombre de la función)
#define LOG_INFO(msg, ...)  Debug::Logger::Get().Log(Debug::Level::Info, __FUNCTION__, msg, __VA_ARGS__)
#define LOG_WARN(msg, ...)  Debug::Logger::Get().Log(Debug::Level::Warning, __FUNCTION__, msg, __VA_ARGS__)
#define LOG_ERROR(msg, ...) Debug::Logger::Get().Log(Debug::Level::Error, __FUNCTION__, msg, __VA_ARGS__)
#endif

namespace Debug {
    enum class Level {
        Info,
        Warning,
        Error,
        Success
    };

    class Logger {
    private:
        std::mutex logMutex; // Candado para los hilos

        // Constructor Privado (Singleton)
        Logger() = default;
        ~Logger() = default;

    public:
        // Singleton Access
        static Logger& Get() {
            static Logger instance;
            return instance;
        }

        Logger(const Logger&) = delete;
        void operator=(const Logger&) = delete;

        // Función principal variádica (acepta argumentos tipo printf)
        template <typename ...Args>
        void Log(Level level, const char* functionName, const std::string& format, Args&& ...args) {
            std::lock_guard<std::mutex> lock(logMutex);

            // 1. Color y Prefijo según el nivel
            const char* colorCode = "";
            const char* levelStr = "";

            switch (level) {
            case Level::Info:
                colorCode = "\033[94m"; // Azul Brillante
                levelStr = "[INFO]";
                break;
            case Level::Warning:
                colorCode = "\033[93m"; // Amarillo
                levelStr = "[WARN]";
                break;
            case Level::Error:
                colorCode = "\033[91m"; // Rojo
                levelStr = "[FAIL]";
                break;
            case Level::Success:
                colorCode = "\033[92m"; // Verde
                levelStr = "[OKAY]";
                break;
            }

            // 2. Timestamp (Hora actual)
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

            struct tm timeInfo; // Creamos una estructura propia para guardar la hora
            localtime_s(&timeInfo, &now); // Usamos la versión segura de Microsoft

            // Pasamos la dirección de nuestra estructura segura (&timeInfo)
            std::cout << "\033[90m[" << std::put_time(&timeInfo, "%H:%M:%S") << "] ";

            // 3. Imprimir Nivel y Función
            std::cout << colorCode << levelStr << " \033[37m" << functionName << ": ";

            // 4. Imprimir Mensaje Formateado (Simple)
            // Nota: Para formateo complejo como fmt::format se requiere C++20 o librería externa.
            // Aquí haremos una impresión directa simple para compatibilidad.
            std::cout << format;
            ((std::cout << " " << std::forward<Args>(args)), ...); // Fold expression (C++17) para imprimir argumentos extra

            // 5. Resetear color
            std::cout << "\033[0m" << std::endl;
        }
    };
}