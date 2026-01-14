#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <jni.h>
#include "../env.h"

#include "../../utils/xorstr.h"

namespace TheVoid {
    namespace Java {

        class Mapper {
        public:
            // El diccionario final que usará el juego
            static std::unordered_map<std::string, std::string> classes;
            static std::unordered_map<std::string, std::string> methods;
            static std::unordered_map<std::string, std::string> fields;

            // NUEVO: Mapa para guardar las firmas que sí funcionaron
            static std::unordered_map<std::string, std::string> fieldSigs;

            // Función maestra
            static void Init();

        private:
            static void ResolveClass(std::string internalName, std::vector<std::string> candidates);

            // AHORA ACEPTA LISTA DE FIRMAS
            static void ResolveField(std::string classKey, std::string fieldKey, std::vector<std::string> candidates, std::vector<std::string> sigCandidates);

            static void ResolveMethod(std::string classKey, std::string methodKey, std::vector<std::string> candidates, std::string signature);
        };
    }
}
