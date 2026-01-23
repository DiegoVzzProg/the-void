#include <windows.h>
#include <iostream>
#include <thread>

#include "../data/env.h"
#include "../data/web/web.h"
#include "../dll/debug.h"

namespace Main {
	class CMain {
	private:
		inline static bool panic = false;
	public:
		static void Init(HMODULE hModule);
	};
}