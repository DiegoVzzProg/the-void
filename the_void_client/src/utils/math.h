#pragma once
#include <random>
#include "../utils/obf.h"
#include "../utils/JunkCode.h"

#define M_PI 3.14159265358979323846

namespace TheVoid {
    struct Vector3 {
        OBF(double) x, y, z;

        Vector3(double _x = 0, double _y = 0, double _z = 0) : x(_x), y(_y), z(_z) {}

        // Operaciones Vectoriales
        Vector3 operator+(const Vector3& o) const { return Vector3(x + o.x, y + o.y, z + o.z); }
        Vector3 operator-(const Vector3& o) const { return Vector3(x - o.x, y - o.y, z - o.z); }
        Vector3 operator*(double f) const { return Vector3(x * f, y * f, z * f); }

        double Dist(const Vector3& o) const {
            double dx = x - o.x; double dy = y - o.y; double dz = z - o.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }

        // Producto Punto (Para ángulos)
        double Dot(const Vector3& o) const { return x * o.x + y * o.y + z * o.z; }

        // Longitud del vector
        double Length() const { return sqrt(x * x + y * y + z * z); }

        // Normalizar (Hacer que la longitud sea 1)
        Vector3 Normalize() const {
            double l = Length();
            if (l == 0) return *this;
            return *this * (1.0 / l);
        }
    };
    namespace Math {
        // Genera un número aleatorio entre min y max
        inline int RandomInt(int min, int max) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(min, max);
            return distrib(gen);
        }

        // Normalizar ángulos (-180 a 180)
        static float WrapAngleTo180(float angle) {
            angle = fmod(angle, 360.0f);
            if (angle >= 180.0f) angle -= 360.0f;
            if (angle < -180.0f) angle += 360.0f;
            return angle;
        }

        // Calcular los ángulos necesarios para mirar desde 'me' hasta 'target'
        static void GetRotations(Vector3 me, Vector3 target, float* outYaw, float* outPitch) {
            double dx = target.x - me.x;
            double dy = target.y - me.y; // Ojo: target.y debe ser la cabeza (eyePos)
            double dz = target.z - me.z;

            double dist = sqrt(dx * dx + dz * dz);

            float yaw = (float)(atan2(dz, dx) * 180.0 / M_PI) - 90.0f;
            float pitch = (float)(-(atan2(dy, dist) * 180.0 / M_PI));

            *outYaw = WrapAngleTo180(yaw);
            *outPitch = pitch;
        }

        static float RandomFloat(float min, float max) {
            return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
        }
    }
}
