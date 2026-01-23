#pragma once
#include <string>
#include <vector>

namespace Helper {
	class Expert {
	public:
		static void ResolveFieldGeneric(std::string classKey, std::string fieldKey, std::vector<std::string> nameCandidates, std::string expectedTypeKey = "");

		static void ResolveMethodGeneric(std::string classKey, std::string methodKey, std::vector<std::string> nameCandidates, std::string returnTypeKey = "", int argsCount = 0);
	};
}