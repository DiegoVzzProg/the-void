#pragma once
#include <chrono>
#include <random>
#include "../../java/sdk/Minecraft.h"
#include "../../utils/math.h"
#include "../../utils/xorstr.h"

namespace TheVoid {
    namespace Modules {

        class LeftAutoClicker {
        private:
            // Configuración (esto luego vendrá de una GUI)
            bool enabled = true;
            int minCPS = 9;
            int maxCPS = 14;

            // Variables internas de tiempo
            std::chrono::steady_clock::time_point lastClickTime;
            long long nextDelay = 0;

            bool IsGameFocused() {
                // 1. Obtener la ventana que el usuario está viendo actualmente
                HWND hForeground = GetForegroundWindow();
                if (!hForeground) return false;

                // 2. Obtener el ID del proceso dueño de esa ventana
                DWORD foregroundProcessId;
                GetWindowThreadProcessId(hForeground, &foregroundProcessId);

                // 3. Compararlo con NUESTRO ID (donde estamos inyectados)
                return (foregroundProcessId == GetCurrentProcessId());
            }

            // Generador Gaussiano para humanización
            // Retorna un delay en ms basado en una distribución normal
            long long GetHumanDelay() {
                static std::random_device rd;
                static std::mt19937 gen(rd());

                // Calculamos la media de CPS deseada
                double avgCPS = (minCPS + maxCPS) / 2.0;
                double meanDelay = 1000.0 / avgCPS;

                // Desviación estándar (qué tanto se aleja del centro). 
                // Un valor alto = clicks muy irregulares. Un valor bajo = clicks robóticos.
                // 15.0ms es un buen punto medio "Legit".
                std::normal_distribution<> d(meanDelay, 15.0);

                long long delay = static_cast<long long>(d(gen));

                // Límites duros para que no clickee a 50 CPS por error matemático
                long long minDelay = 1000 / (maxCPS + 3);
                long long maxDelay = 1000 / (minCPS - 3);

                if (delay < minDelay) delay = minDelay;
                if (delay > maxDelay) delay = maxDelay;

                return delay;
            }


        public:
            // Singleton: Para acceder a la misma instancia siempre
            static LeftAutoClicker& Instance() {
                static LeftAutoClicker instance;
                return instance;
            }

            void Update() {
                if (!IsGameFocused()) {
                    return;
                }
                if (!enabled) return;

                // 1. Trigger: ¿Está presionado el Click Izquierdo?
                if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                    // Reseteamos el delay para que el primer click al volver a presionar sea instantáneo
                    nextDelay = 0;
                    return;
                }

                auto mc = SDK::Minecraft::GetMinecraft();
                if (!mc.IsValid()) {
                    std::cout << "[DEBUG] Minecraft no valido" << std::endl;
                    return;
                }

                // 2. Safety: ¿Estamos en inventario?
                if (mc.InGui()) {
                    std::cout << "[DEBUG] BLOQUEADO: Inventario abierto (InGui true)" << std::endl;
                    Java::env->DeleteLocalRef(mc.GetInstance());
                    return;
                }

                // 3. Smart Mining: ¿Estamos mirando un bloque?
                // Si miramos un bloque, NO clickeamos, dejamos que Minecraft mine nativamente.
                if (mc.IsLookingAtBlock()) {
                    std::cout << "\r[DEBUG] BLOQUEADO: Mirando Bloque (Smart Mining)   " << std::flush;
                    Java::env->DeleteLocalRef(mc.GetInstance());
                    return;
                }

                // 4. Lógica de Tiempo
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime).count();

                if (elapsed >= nextDelay) {
                    // Intentando golpear...
                    std::cout << "[DEBUG] INTENTO DE GOLPE (ClickMouse)... " << std::flush;

                    mc.SetLeftClickCounter(0);
                    mc.ClickMouse();

                    std::cout << "EJECUTADO!" << std::endl;

                    // Recalcular el próximo delay usando matemáticas humanas
                    nextDelay = GetHumanDelay();
                    lastClickTime = now;
                }

                Java::env->DeleteLocalRef(mc.GetInstance());
            }

        };
    }
}