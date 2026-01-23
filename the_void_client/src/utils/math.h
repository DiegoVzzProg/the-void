#pragma once
#include <random>
#include <cmath>
#include "../utils/obf.h"
#include "../utils/JunkCode.h"

#define M_PI 3.14159265358979323846
#define DEG2RAD(x) ((float)(x) * (float)(M_PI / 180.f))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI))

namespace TheVoid {
    // Estructura Vector2 (NECESARIA para Ángulos Yaw/Pitch)
    struct Vector2 {
        float x, y;
        Vector2(float _x = 0.f, float _y = 0.f) : x(_x), y(_y) {}
        Vector2 operator-(const Vector2& o) const { return Vector2(x - o.x, y - o.y); }
        Vector2 operator+(const Vector2& o) const { return Vector2(x + o.x, y + o.y); }
    };

    struct Vector3 {
        OBF(double) x, y, z;
        Vector3(double _x = 0, double _y = 0, double _z = 0) : x(_x), y(_y), z(_z) {}

        Vector3 operator+(const Vector3& o) const { return Vector3(x + o.x, y + o.y, z + o.z); }
        Vector3 operator-(const Vector3& o) const { return Vector3(x - o.x, y - o.y, z - o.z); }
        Vector3 operator*(double f) const { return Vector3(x * f, y * f, z * f); }

        double Dist(const Vector3& o) const {
            double dx = x - o.x; double dy = y - o.y; double dz = z - o.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }

        double Length() const { return sqrt(x * x + y * y + z * z); }
    };

    namespace Math {
        inline int RandomInt(int min, int max) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(min, max);
            return distrib(gen);
        }

        inline float RandomFloat(float min, float max) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(min, max);
            return dis(gen);
        }

        inline float WrapAngleTo180(float angle) {
            angle = fmod(angle, 360.0f);
            if (angle >= 180.0f) angle -= 360.0f;
            if (angle < -180.0f) angle += 360.0f;
            return angle;
        }

        
        inline Vector2 GetRotations(Vector3 me, Vector3 target) {
            double dx = target.x - me.x;
            double dy = target.y - me.y;
            double dz = target.z - me.z;
            double dist = sqrt(dx * dx + dz * dz);

            float yaw = (float)(atan2(dz, dx) * 180.0 / M_PI) - 90.0f;
            float pitch = (float)(-(atan2(dy, dist) * 180.0 / M_PI));

            return Vector2(WrapAngleTo180(yaw), pitch);
        }
    }
}