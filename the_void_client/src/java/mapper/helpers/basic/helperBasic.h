#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "../../Mapper.h"
#include "../../../env.h"
#include "../../../../utils/JavaUtils.h"
#include "../../../../utils/xorstr.h"

namespace TheVoid {
	namespace Helper {
		class Basic {
		public:
			static void ResolveClass(std::string internalName, std::vector<std::string> candidates);

			static void ResolveField(std::string classKey, std::string fieldKey, std::vector<std::string> candidates, std::vector<std::string> sigCandidates);

			static void ResolveMethod(std::string classKey, std::string methodKey, std::vector<std::string> candidates, std::string signature);
		};
	}
}