#pragma once
#include <iostream>
#include <thread>
#include <chrono>

#ifndef IXWEBSOCKET_STATIC
#define IXWEBSOCKET_STATIC
#endif

#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXNetSystem.h>

#include <nlohmann/json.hpp>
#include <mutex>

#include "../encrypt/encrypt.h"

#include "../../dll/debug.h"

using json = nlohmann::json;

namespace Online {
	class CWeb {
	private:
		inline static ix::WebSocket clientSocket;
		inline static std::mutex candado;
		inline static json module_manager_value = nullptr;
		inline static bool is_new_command = false;
	public:
		static void Init();
		static void CleanUp();
		static bool ModuleActivated(std::string key);
		static auto GetModuleValue(std::string key);
	};
}
