#pragma once
#include <ixwebsocket/IXWebSocket.h>

#include <nlohmann/json.hpp>
#include <mutex>

struct Web {
	using json = nlohmann::json;

	static void Init();
	static void Cleanup();

	inline static std::mutex candado;

	inline static json module_manager_value;
	inline static bool is_new_command = false;
};