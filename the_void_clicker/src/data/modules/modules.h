#pragma once
#include "./leftclicker/leftclicker.h"
#include "./aimassist/aimassist.h"

namespace Feature {
	class CModules {
	private:
		CModules() = default;
	public:
		static CModules& Get() {
			static CModules instance;
			return instance;
		}

		CModules(const CModules&) = delete;
		void operator=(const CModules&) = delete;

		void Activated();
	};
}