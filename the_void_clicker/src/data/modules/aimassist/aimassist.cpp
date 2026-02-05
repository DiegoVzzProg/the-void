#include "aimassist.h"
#include <vector>
#include <iostream>
#include <algorithm>

namespace Combat {

    float AimAssist::GetRandomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    float AimAssist::WrapAngleTo180(float angle) {
        angle = fmod(angle, 360.0f);
        if (angle >= 180.0f) angle -= 360.0f;
        if (angle < -180.0f) angle += 360.0f;
        return angle;
    }

    float AimAssist::GetDistance(Vector3 a, Vector3 b) {
        return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
    }

    float AimAssist::ApplyGCD(float delta, float sensitivity) {
        float f = sensitivity * 0.6f + 0.2f;
        float gcd = f * f * f * 1.2f;

        float remainder = std::fmod(delta, gcd);
        return delta - remainder;
    }

    Vector2 AimAssist::GetRotationsNeeded(Vector3 myEyePos, Vector3 targetPos) {
        double diffX = targetPos.x - myEyePos.x;
        double diffZ = targetPos.z - myEyePos.z;
        double diffY = targetPos.y - myEyePos.y;
        double dist = sqrt(diffX * diffX + diffZ * diffZ);
        float yaw = (float)(atan2(diffZ, diffX) * 180.0 / M_PI) - 90.0f;
        float pitch = (float)-(atan2(diffY, dist) * 180.0 / M_PI);
        return { WrapAngleTo180(yaw), pitch };
    }

    // EN aimassist.cpp

    void AimAssist::Activate() {
        std::string modName = xorstr("aim-assists");
        if (!Online::CWeb::Get().IsModuleActive(modName)) return;
        if (!Java::CFunctions::Get().IsIngame() || Java::CFunctions::Get().InGui()) return;

        if (GetRandomFloat(0.0f, 100.0f) < 15.0f) return;

        // --- CARGAR CONFIG ---
        fov = Online::CWeb::Get().GetValue(modName, xorstr("fov"));
        range = Online::CWeb::Get().GetValue(modName, xorstr("range"));
        smoothness = Online::CWeb::Get().GetValue(modName, xorstr("smoothness"));                             
        clickOnly = Online::CWeb::Get().ActivatedCondition(modName, xorstr("click_only"));
        break_block = Online::CWeb::Get().ActivatedCondition(modName, xorstr("break_blocks"));
        strafeAssist = Online::CWeb::Get().ActivatedCondition(modName, xorstr("strafe_assist"));

        if (fov < 1.0f) fov = 180.0f;
        if (range < 1.0f) range = 6.0f;
        if (smoothness < 1.0f) smoothness = 1.0f;

        if(break_block && Java::CFunctions::Get().IsLookingAtBlock()) return;
        if (Java::CFunctions::Get().IsLookingAtEntity()) return;

        jobject me = Java::CFunctions::Get().GetPlayer();
        if (!me) return;

        // ====================================================================
            // LÓGICA DE INTENSIDAD BASADA EN ANIMACIÓN (Realista)
            // ====================================================================
        static float aimIntensity = 0.0f;

        // 1. LEEMOS LA ANIMACIÓN REAL DEL JUEGO
        // swingProgress va de 0.0 (reposo) a 1.0 (golpe finalizado)
        float swing = Java::CFunctions::Get().GetSwingProgress(me);

        // También leemos el click por si acaso la animación tarda en empezar
        bool isClicking = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

        bool shouldAim = true;

        if (clickOnly) {
            // Si clickOnly está activo, apuntamos si:
            // A) Estamos presionando el botón
            // B) O el brazo se sigue moviendo (swing > 0)
            // Esto une perfectamente los clicks y evita el "temblor".
            if (!isClicking && swing <= 0.0f) {
                shouldAim = false;
            }
        }

        // 2. FADING (Suavizado de entrada/salida)
        if (shouldAim) {
            aimIntensity += 0.15f; // Sube rápido al iniciar el golpe
        }
        else {
            aimIntensity -= 0.05f; // Baja lento al terminar (Decay suave)
        }

        // Clamp
        if (aimIntensity > 1.0f) aimIntensity = 1.0f;
        if (aimIntensity < 0.0f) aimIntensity = 0.0f;

        // Si la intensidad es 0, salimos.
        if (aimIntensity <= 0.01f) {
            Java::CEnv::Get().env->DeleteLocalRef(me);
            return;
        }
        // ====================================================================

        Vector3 myPos = Java::CFunctions::Get().GetEntityPos(me);
        Vector3 headPos = Java::CFunctions::Get().GetEyePos(me);
        Vector2 myRot = Java::CFunctions::Get().GetMyRotation();
        Java::CEnv::Get().env->DeleteLocalRef(me);

        std::vector<jobject> players = Java::CFunctions::Get().GetWorldPlayers();
        jobject bestTarget = nullptr;
        float bestFovScore = fov;

        for (jobject target : players) {
            if (!target) continue;

            Vector3 targetPos = Java::CFunctions::Get().GetEntityPos(target);

            if ((targetPos.x == myPos.x && targetPos.z == myPos.z) || (targetPos.x == 0 && targetPos.z == 0)) {
                continue;
            }

            float dist = GetDistance(myPos, targetPos);

            if (dist > range || dist < 0.5f) continue;

            float entH = Java::CFunctions::Get().GetHeight(target);
            if (entH <= 0.0f) entH = 1.8f;

            Vector3 centerPos = targetPos; centerPos.y += entH * 0.5f;
            Vector2 targetRot = GetRotationsNeeded(headPos, centerPos);

            float yawDiff = std::abs(WrapAngleTo180(targetRot.yaw - myRot.yaw));
            float pitchDiff = std::abs(WrapAngleTo180(targetRot.pitch - myRot.pitch));
            float totalFovDiff = sqrt(yawDiff * yawDiff + pitchDiff * pitchDiff);

            if (totalFovDiff < bestFovScore) {
                bestFovScore = totalFovDiff;
                bestTarget = target;
            }
        }

        jobject finalTarget = nullptr;
        if (bestTarget)
            finalTarget = Java::CEnv::Get().env->NewLocalRef(bestTarget);
        

        for (jobject p : players) if (p) Java::CEnv::Get().env->DeleteLocalRef(p);
        players.clear();

        if (!finalTarget) return;

        float entH = Java::CFunctions::Get().GetHeight(finalTarget);
        Vector3 aimPos = Java::CFunctions::Get().GetEntityPos(finalTarget);

        //if (boneChangeTimer++ > 30) {
        //    humanBoneY = GetRandomFloat(0.50f, 0.70f);
        //    humanBoneX = GetRandomFloat(-0.1f, 0.1f);
        //    humanBoneZ = GetRandomFloat(-0.1f, 0.1f);
        //    boneChangeTimer = 0;
        //}

        //aimPos.y += (entH * 0.60f);
        //aimPos.x += GetRandomFloat(-noiseXY, noiseXY);
        //aimPos.z += GetRandomFloat(-noiseXY, noiseXY);

        static float driftX = 0, driftY = 0;
        static int driftTimer = 0;
        if (driftTimer++ > 15) {
            driftX = GetRandomFloat(-0.1f, 0.1f);
            driftY = GetRandomFloat(-0.1f, 0.2f);
            driftTimer = 0;
        }
        aimPos.y += (entH * 0.55f) + driftY;
        aimPos.x += driftX;
        aimPos.z += driftX;

        Vector2 needed = GetRotationsNeeded(headPos, aimPos);

        if (strafeAssist) {
            if (GetAsyncKeyState('D') & 0x8000) needed.yaw -= strafeOffset;
            if (GetAsyncKeyState('A') & 0x8000) needed.yaw += strafeOffset;
        }

        float deltaYaw = WrapAngleTo180(needed.yaw - myRot.yaw);
        float deltaPitch = WrapAngleTo180(needed.pitch - myRot.pitch);

        float totalDiff = sqrt(deltaYaw * deltaYaw + deltaPitch * deltaPitch);

        if (totalDiff < 1.0f) {
            Java::CEnv::Get().env->DeleteLocalRef(finalTarget);
            return;
        }

        // --- VELOCIDAD ---
        float baseSmooth = smoothness * 1.5f;

        // Frenado
        if (totalDiff < 6.0f) baseSmooth += (10.0f - totalDiff) * 12.0f;

        // Randomización
        float randomSpeed = GetRandomFloat(0.9f, 1.1f);

        float yawSmooth = baseSmooth * randomSpeed;
        float pitchSmooth = (baseSmooth * 1.35f) * randomSpeed;

        if (yawSmooth < 1.0f) yawSmooth = 1.0f;
        if (pitchSmooth < 1.0f) pitchSmooth = 1.0f;

        float moveYaw = deltaYaw / yawSmooth;
        float movePitch = deltaPitch / pitchSmooth;

        // APLICACIÓN DE INTENSIDAD (Aquí usamos el valor real de la animación)
        moveYaw *= aimIntensity;
        movePitch *= aimIntensity;

        float sensitivity = Java::CFunctions::Get().GetMouseSensitivity();
        if (sensitivity <= 0.001f) sensitivity = 0.5f;

        moveYaw = ApplyGCD(moveYaw, sensitivity);
        movePitch = ApplyGCD(movePitch, sensitivity);

        myRot.yaw += moveYaw;
        myRot.pitch += movePitch;

        myRot.yaw = WrapAngleTo180(myRot.yaw);
        if (myRot.pitch > 90.0f) myRot.pitch = 90.0f;
        if (myRot.pitch < -90.0f) myRot.pitch = -90.0f;

        Java::CFunctions::Get().SetMyRotation(myRot);
        Java::CEnv::Get().env->DeleteLocalRef(finalTarget);
        //float finalSmoothness = smoothness * 1.5f;

        //if (totalDiff < 6.0f) {
        //    finalSmoothness += (10.0f - totalDiff) * 12.0f;
        //}

        //finalSmoothness *= GetRandomFloat(0.9f, 1.1f);
        //if (finalSmoothness < 1.0f) finalSmoothness = 1.0f;

        //float moveYaw = deltaYaw / finalSmoothness;
        //float movePitch = deltaPitch / finalSmoothness;

        //float sensitivity = Java::CFunctions::Get().GetMouseSensitivity();
        //if (sensitivity <= 0.001f) sensitivity = 0.5f;

        //moveYaw = ApplyGCD(moveYaw, sensitivity);
        //movePitch = ApplyGCD(movePitch, sensitivity);

        //myRot.yaw += moveYaw;
        //myRot.pitch += movePitch;

        //// Clamp Pitch
        //if (myRot.pitch > 90.0f) myRot.pitch = 90.0f;
        //if (myRot.pitch < -90.0f) myRot.pitch = -90.0f;

        //myRot.yaw = WrapAngleTo180(myRot.yaw);

        //Java::CFunctions::Get().SetMyRotation(myRot);
        //Java::CEnv::Get().env->DeleteLocalRef(finalTarget);
    }
}