#pragma once
#include <string>
#include <unordered_map>
#include <vector>



namespace Java {

    class Mapper {
    public:
        static std::unordered_map<std::string, std::string> classes;
        static std::unordered_map<std::string, std::string> methods;
        static std::unordered_map<std::string, std::string> fields;
        static std::unordered_map<std::string, std::string> fieldSigs;
        static std::unordered_map<std::string, std::string> methodSigs;
        static std::unordered_map<std::string, jclass> classCache;

        static void Init();

        static bool LearnAndCheck(jobject obj, std::string cacheKey, std::vector<std::string> searchPatterns);
    private:
        static void FindRealMOPClass();
    };
}