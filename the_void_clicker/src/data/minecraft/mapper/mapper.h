#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "./helpers/basic/helperBasic.h"
#include "./helpers/expert/helperExpert.h"
#include "../../env.h"
#include "../../utils/JavaUtils.h"
#include "../../../dll/debug.h"

namespace Java {

    class CMapper {
    private:
        CMapper() = default;
        ~CMapper() { Cleanup(); }
        
        void FindRealMOPClass();
    public:
        // 2. Eliminamos constructores de copia (Regla del Singleton)
        CMapper(const CMapper&) = delete;
        void operator=(const CMapper&) = delete;

        // 3. El método de acceso estático (Singleton)
        static CMapper& Get() {
            static CMapper instance;
            return instance;
        }

        std::unordered_map<std::string, std::string> classes;
        std::unordered_map<std::string, std::string> methods;
        std::unordered_map<std::string, std::string> fields;
        std::unordered_map<std::string, std::string> fieldSigs;
        std::unordered_map<std::string, std::string> methodSigs;
        std::unordered_map<std::string, jclass> classCache;

        void Init();
        void Cleanup();

        bool LearnAndCheck(jobject obj, std::string cacheKey, std::vector<std::string> searchPatterns);
    };
}