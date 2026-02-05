#pragma once
#include "../functions.h" 
#include "../../web/web.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace Combat {
    class AimAssist {
    private:
        AimAssist() = default;

        bool enabled = false;
        bool clickOnly = true;
        bool strafeAssist = false;
        bool break_block = false;

        float humanBoneX = 0;
        float humanBoneY = 0;
        float humanBoneZ = 0;
        int boneChangeTimer = 0;

        float fov = 60.0f;
        float smoothness = 40.0f;
        float range = 4.0f;
        float strafeOffset = 3.0f;
        float noiseXY = 0.05f;

        float ApplyGCD(float delta, float sensitivity);
        float WrapAngleTo180(float angle);
        Vector2 GetRotationsNeeded(Vector3 myPos, Vector3 targetPos);
        float GetDistance(Vector3 a, Vector3 b);
        float GetRandomFloat(float min, float max);
    public:
        static AimAssist& Get() {
            static AimAssist instance;
            return instance;
        }

        AimAssist(const AimAssist&) = delete;
        void operator=(const AimAssist&) = delete;

        void Activate();
    };
}