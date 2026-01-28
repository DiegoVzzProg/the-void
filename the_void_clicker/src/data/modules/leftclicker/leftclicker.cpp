#include "leftclicker.h"

namespace Left {
    void Clicker::ResetExhaustTimers() {
        auto now = std::chrono::steady_clock::now();

        std::uniform_int_distribution<> timeDist(40, 80);
        nextExhaustTime = now + std::chrono::seconds(timeDist(GetGenerator()));

        isExhausted = false;
    }

    std::mt19937& Clicker::GetGenerator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    long long Clicker::GetHumanDelay(double currentTargetCPS) {
        auto& gen = GetGenerator();

        double meanDelay = 1000.0 / currentTargetCPS;

        double deviation = isExhausted ? 12.0 : 8.0;
        std::normal_distribution<> d(meanDelay, deviation);
        long long delay = static_cast<long long>(d(gen));

        long long minLimit = 1000 / (maxcps + 4);
        long long maxLimit = 1000 / (std::max)(1, mincps - 2);

        if (delay < minLimit) delay = minLimit;
        if (delay > maxLimit) delay = maxLimit;

        return delay;
    }

    long long Clicker::GetHoldTime() {
        double targetCPS = (double)maxcps;
        if (targetCPS < 1) targetCPS = 1;
        long long period = 1000 / (long long)targetCPS;

        long long maxHold = period / 2;
        long long minHold = maxHold - 10;
        if (minHold < 10) minHold = 10; 
        if (maxHold < 20) maxHold = 20;

        if (targetCPS <= 12) {
            minHold = 30;
            maxHold = 60;
        }

        std::uniform_int_distribution<> d(minHold, maxHold);
        return d(GetGenerator());
    }

    long long Clicker::GetNextBlockDelay() {
        std::uniform_int_distribution<> d(800, 1000);
        return d(GetGenerator());
    }

    bool Clicker::FoundGame() {
        if (!Java::CEnv::Get().GameWindow) {
            Utils::FindGameWindow();
            return false;
        }

        if (GetForegroundWindow() != Java::CEnv::Get().GameWindow) {
            return false;
        }

        return true;
    }

    void Clicker::Activate() {
        if (!Online::CWeb::Get().ItIsTheModule(xorstr("auto-clicker")))
            return;

        enabled = Online::CWeb::Get().ModuleActivated();

        if (!enabled) {
            ResetExhaustTimers();
            return;
        }

        if (!FoundGame() || !Java::CFunctions::Get().IsIngame()) {
            return;
        }

        if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            isMining = false;
            return;
        }

        maxcps = Online::CWeb::Get().GetValue(xorstr("cps"));
        mincps = (maxcps > 4) ? maxcps - 3 : 1;
        inventory = Online::CWeb::Get().ActivatedCondition(xorstr("inventory"));
        weapon_check = Online::CWeb::Get().ActivatedCondition(xorstr("weapon_check"));
        ignore_blocks = Online::CWeb::Get().ActivatedCondition(xorstr("ignore_blocks"));

        bool shouldClick = false;

        if (Java::CFunctions::Get().InGui()) {
            isMining = false;
            if (!inventory) return;

            if (Java::CFunctions::Get().IsInInventory()) {
                shouldClick = true;
            }
            else {
                return;
            }

        }
        else {
            if (weapon_check) {
                if (!Java::CFunctions::Get().IsHoldingWeapon()) {
                    isMining = false;
                    return;
                }
            }

            if (ignore_blocks) {
                if (Java::CFunctions::Get().IsLookingAtBlock()) {
                    if (!isMining) {
                        PostMessageA(Java::CEnv::Get().GameWindow, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
                        isMining = true;
                    }
                    return;
                }
            }

            isMining = false;
            shouldClick = true;
        }

        if (!shouldClick) return;

        if (!shouldClick) return;

        // --- LÓGICA DE EXHAUST (NUEVO) ---
        auto now = std::chrono::steady_clock::now();

        if (isExhausted) {
            if (now >= exhaustRecoveryTime) {
                isExhausted = false;

                std::uniform_int_distribution<> intervalDist(45, 90); // Segundos
                nextExhaustTime = now + std::chrono::seconds(intervalDist(GetGenerator()));
            }
        }
        else {

            if (nextExhaustTime.time_since_epoch().count() == 0) {
                ResetExhaustTimers();
            }

            if (now >= nextExhaustTime) {
                isExhausted = true;

                std::uniform_int_distribution<> durDist(3000, 6000);
                exhaustRecoveryTime = now + std::chrono::milliseconds(durDist(GetGenerator()));
            }
        }

        // --- CÁLCULO DE CPS DINÁMICOS ---
        double effectiveCPS = (double)maxcps;

        if (isExhausted) {
            // FACTOR DE FATIGA: Bajamos un 35-40% los CPS
            // Ejemplo: 24 CPS -> ~15 CPS
            effectiveCPS = effectiveCPS * 0.65;

            // Que no baje de 8 CPS para que no sea injugable
            if (effectiveCPS < 8.0) effectiveCPS = 8.0;
        }

        // --- EJECUCIÓN ---
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime).count();

        if (elapsed >= nextDelay) {
            // Click Down
            PostMessageA(Java::CEnv::Get().GameWindow, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));

            // Hold Time también debería ser un poco más "perezoso" si estamos cansados
            long long holdTime = GetHoldTime();
            if (isExhausted) holdTime += 15; // Clicks más largos por cansancio

            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));

            // Click Up
            PostMessageA(Java::CEnv::Get().GameWindow, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));

            lastClickTime = std::chrono::steady_clock::now();

            // AQUÍ USAMOS LOS CPS EFECTIVOS
            long long totalPeriod = GetHumanDelay(effectiveCPS); // <-- Pasamos el CPS calculado

            nextDelay = totalPeriod - holdTime;
            if (nextDelay < 0) nextDelay = 0;
        }
    }

    void Clicker::AutoBlockHit() {
        if (!Online::CWeb::Get().ItIsTheModule(xorstr("auto-clicker"))) return;

        bool inputAttack = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

        if (!enabled || !FoundGame() || !inputAttack || Java::CFunctions::Get().InGui()) {
            if (isBlockingActive) {
                Java::CFunctions::Get().SetRightClickState(false);
                isBlockingActive = false;
            }
            return;
        }

        autoBlockHit = Online::CWeb::Get().ActivatedCondition(xorstr("auto_block_hit"));
        if (!autoBlockHit) return;

        bool canBlock = Java::CFunctions::Get().IsHoldingSword() &&
            Java::CFunctions::Get().IsLookingAtEntity();

        if (!canBlock) {
            if (isBlockingActive) {
                Java::CFunctions::Get().SetRightClickState(false);
                isBlockingActive = false;
            }
            return;
        }

        auto now = std::chrono::steady_clock::now();

        if (isBlockingActive) {
            if (now >= blockReleaseTime) {

                Java::CFunctions::Get().SetRightClickState(false);
                isBlockingActive = false;

                lastBlockTime = now;
                nextBlockDelay = GetNextBlockDelay();
            }
        }
        else {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlockTime).count();

            if (elapsed >= nextBlockDelay) {
                std::uniform_int_distribution<> d(0, 100);

                if (d(GetGenerator()) <= blockChance) {
                    Java::CFunctions::Get().SetRightClickState(true);
                    isBlockingActive = true;

                    std::uniform_int_distribution<> durVar(-15, 15);
                    int duration = blockDuration + durVar(GetGenerator());
                    if (duration < 20) duration = 20;

                    blockReleaseTime = now + std::chrono::milliseconds(duration);
                }
                else {
                    lastBlockTime = now;
                    nextBlockDelay = 100;
                }
            }
        }
    }
}