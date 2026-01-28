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

#include "../../dll/debug.h"
#include "../encrypt/encrypt.h"


using json = nlohmann::json;

namespace Online {
	class CWeb {
	private:
		CWeb() = default;
		~CWeb() { CleanUp(); }

		ix::WebSocket clientSocket;
		std::mutex candado;
		json module_manager_value = nullptr;
		bool is_new_command = false;

		std::vector<std::string> cached_conditions;
	public:
		static CWeb& Get() {
			static CWeb instance;
			return instance;
		}

		CWeb(const CWeb&) = delete;
		void operator=(const CWeb&) = delete;

		void Init();
		void CleanUp();
		void AcknowledgeCommand();
		
		bool ItIsTheModule(std::string key);
		bool ModuleActivated();
		bool ActivatedCondition(std::string key);

		json GetValue(std::string key);
		json GetModuleValue(std::string key);
	};
}
