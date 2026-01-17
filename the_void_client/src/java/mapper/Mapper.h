#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <jni.h>
#include "../env.h"
#include "../../utils/JavaUtils.h"
#include "../../utils/xorstr.h"

#include "./helpers/basic/helperBasic.h"
#include "./helpers/expert/helperExpert.h"

namespace TheVoid {
    namespace Java {

        class Mapper {
        public:
            // El diccionario final que usará el juego
            static std::unordered_map<std::string, std::string> classes;
            static std::unordered_map<std::string, std::string> methods;
            static std::unordered_map<std::string, std::string> fields;
            static std::unordered_map<std::string, std::string> fieldSigs;
            static std::unordered_map<std::string, std::string> methodSigs;
            static std::unordered_map<std::string, jclass> classCache;

            // Función maestra
            static void Init();

            static bool LearnAndCheck(jobject obj, std::string cacheKey, std::vector<std::string> searchPatterns);
        private:
            static void FindRealMOPClass();
        };
    }
}
