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
		class Expert {
		public:
			static void ResolveFieldGeneric(std::string classKey, std::string fieldKey, std::vector<std::string> nameCandidates, std::string expectedTypeKey = "");

			static void ResolveMethodGeneric(std::string classKey, std::string methodKey, std::vector<std::string> nameCandidates, std::string returnTypeKey = "", int argsCount = 0);
		};
	}
}