#pragma once
#include <random>

namespace TheVoid {
    namespace Math {
        // Genera un número aleatorio entre min y max
        inline int RandomInt(int min, int max) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(min, max);
            return distrib(gen);
        }
    }
}
