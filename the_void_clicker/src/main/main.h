#define WIN32_LEAN_AND_MEAN // 1. Desactivar basura vieja de Windows
#include <winsock2.h>       // 2. Cargar Sockets Nuevos
#include <windows.h>        // 3. Cargar Windows

#include <iostream>
#include <thread>

#include "../data/env.h"
#include "../data/web/web.h"
#include "../dll/debug.h"
#include "../data/encrypt/encrypt.h"
#include "../data/modules/modules.h"

namespace Main {
	class CMain {
	private:
		CMain() = default;
		
		bool panic = false;
	public:
		static CMain& Get() {
			static CMain instance;
			return instance;
		}

		CMain(const CMain&) = delete;
		void operator=(const CMain&) = delete;

		void Init(HMODULE hModule);
	};
}