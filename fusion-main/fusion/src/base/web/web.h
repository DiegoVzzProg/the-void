#pragma once
#include <ixwebsocket/IXWebSocket.h>

#include <nlohmann/json.hpp>
#include <mutex>

struct Web {
	using json = nlohmann::json;

	static void Init();
	static void Cleanup();

	static bool ActivatedCondition(std::string key);

	inline static std::mutex candado;

	inline static json module_manager_value = nullptr;
	inline static bool is_new_command = false;
};