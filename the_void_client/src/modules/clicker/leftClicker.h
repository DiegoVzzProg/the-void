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
            int minCPS = (maxCPS > 4) ? maxCPS - 3 : 1;
            int maxCPS = 24;
            bool inventory = true;
            bool weaponOnly = true;

            // Variables internas de tiempo
            std::chrono::steady_clock::time_point lastClickTime;
            long long nextDelay = 0;

            std::mt19937& GetGenerator() {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                return gen;
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

            long long GetHoldTime() {

                std::uniform_int_distribution<> d(30, 60);
                return d(GetGenerator());
            }
        public:
            // Singleton: Para acceder a la misma instancia siempre
            static LeftAutoClicker& Instance() {
                static LeftAutoClicker instance;
                return instance;
            }

            void Update() {
                if (!enabled) return;

                if (!TheVoid::Java::GameWindow) {
                    TheVoid::Utils::FindGameWindow();
                    return;
                }

                if (GetForegroundWindow() != TheVoid::Java::GameWindow) {
                    return;
                }

                if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                    nextDelay = 0;
                    return;
                }

                auto mc = SDK::Minecraft::GetMinecraft();
                if (!mc.IsValid()) {
                    return;
                }
                
                if (mc.InGui()) {
                    if (!inventory) {
                        Java::env->DeleteLocalRef(mc.GetInstance());
                        return;
                    }

                    if (!mc.IsInInventory()) {
                        Java::env->DeleteLocalRef(mc.GetInstance());
                        return;
                    }
                }

                if (mc.IsLookingAtBlock()) {
                    Java::env->DeleteLocalRef(mc.GetInstance());
                    return;
                }

                if (weaponOnly) {
                    if (!mc.IsHoldingWeapon()) {
                        Java::env->DeleteLocalRef(mc.GetInstance());
                        return;
                    }
                }

                // 4. Lógica de Tiempo
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime).count();

                if (elapsed >= nextDelay) {
                    PostMessageA(TheVoid::Java::GameWindow, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
                    long long holdTime = GetHoldTime();
                    std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));

                    PostMessageA(TheVoid::Java::GameWindow, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));

                    nextDelay = GetHumanDelay() - holdTime;
                    if (nextDelay < 0) nextDelay = 0;
                    lastClickTime = now;
                }

                Java::env->DeleteLocalRef(mc.GetInstance());
            }

        };
    }
}