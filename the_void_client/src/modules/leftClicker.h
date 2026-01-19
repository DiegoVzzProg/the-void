#pragma once
#include <chrono>
#include <random>
#include "../java/sdk/Minecraft.h"
#include "../utils/math.h"
#include "../utils/xorstr.h"
#include "../utils/obf.h"
#include "../utils/JunkCode.h"

namespace TheVoid {
    namespace Modules {

        class LeftAutoClicker {
        private:
            // Configuración (esto luego vendrá de una GUI)
            bool enabled = true;
            OBF(int) maxCPS = 24;
            OBF(int) minCPS = (maxCPS > 4) ? maxCPS - 3 : 1;
            bool inventory = true;
            bool weaponOnly = true;
            bool autoBlock = true;
            OBF(int) blockChance = 85;
            OBF(int) blockDuration = 60;

            // Variables internas de tiempo
            std::chrono::steady_clock::time_point lastClickTime;
            OBF(long long) nextDelay = 0;

            std::chrono::steady_clock::time_point lastBlockTime;
            OBF(long long) nextBlockDelay = 0; // Espera entre bloqueos (Cooldown)

            std::mt19937& GetGenerator() {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                return gen;
            }

            // Generador Gaussiano para humanización
            long long GetHumanDelay() {
                static std::random_device rd;
                static std::mt19937 gen(rd());

                double avgCPS = (minCPS + maxCPS) / 2.0;
                double meanDelay = 1000.0 / avgCPS;

                std::normal_distribution<> d(meanDelay, 15.0);

                long long delay = static_cast<long long>(d(gen));

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

            long long GetNextBlockDelay() {
                std::uniform_int_distribution<> d(800, 1000);
                return d(GetGenerator());
            }
        public:
            static LeftAutoClicker& Instance() {
                static LeftAutoClicker instance;
                return instance;
            }

            bool FoundGame() {
                if (!TheVoid::Java::GameWindow) {
                    TheVoid::Utils::FindGameWindow();
                    return false;
                }

                if (GetForegroundWindow() != TheVoid::Java::GameWindow) {
                    return false;
                }

                return true;
            }

            void Update() {
                POLYMORPH();

                if (!enabled) return;

                if (!FoundGame()) {
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

            void AutoBlockUpdate() {
                auto mc = SDK::Minecraft::GetMinecraft();

                if (!autoBlock || !enabled) {
                    Java::env->DeleteLocalRef(mc.GetInstance());
                    return;
                }

                if (!mc.InGui()) {
                    Java::env->DeleteLocalRef(mc.GetInstance());
                    return;
                }

                if((GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                    Java::env->DeleteLocalRef(mc.GetInstance());
                    return;
                }

                if (!(mc.IsHoldingSword() && mc.IsLookingAtEntity())) {
                    Java::env->DeleteLocalRef(mc.GetInstance());
                    return;
                }

                auto now = std::chrono::steady_clock::now();
                auto elapsedBlock = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlockTime).count();

                if (elapsedBlock >= nextBlockDelay) {
                    std::uniform_int_distribution<> d(0, 100);
                    if (d(GetGenerator()) <= blockChance) {

                        std::this_thread::sleep_for(std::chrono::milliseconds(50));

                        PostMessageA(TheVoid::Java::GameWindow, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(0, 0));

                        std::this_thread::sleep_for(std::chrono::milliseconds(blockDuration));

                        PostMessageA(TheVoid::Java::GameWindow, WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(0, 0));

                        lastBlockTime = std::chrono::steady_clock::now();
                        nextBlockDelay = GetNextBlockDelay();
                    }
                }
                

                Java::env->DeleteLocalRef(mc.GetInstance());
            }
        };
    }
}