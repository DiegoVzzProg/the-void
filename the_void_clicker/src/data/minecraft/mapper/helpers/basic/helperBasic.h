#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "../../Mapper.h"
#include "../../../../env.h"
#include "../../../../utils/javautils.h"
#include "../../../../../dll/debug.h"


namespace Helper {
	class Basic {
	public:
		static void ResolveClass(std::string internalName, std::vector<std::string> candidates);

		static void ResolveField(std::string classKey, std::string fieldKey, std::vector<std::string> candidates, std::vector<std::string> sigCandidates);

		static void ResolveMethod(std::string classKey, std::string methodKey, std::vector<std::string> candidates, std::string signature);
	};
}