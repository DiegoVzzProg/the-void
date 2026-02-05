#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
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

		std::map<std::string, json> module_cache;
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
		// Verifica si un módulo específico está encendido en el cache
		bool IsModuleActive(std::string moduleName);

		// Verifica si una condición (checkbox) está activa en un módulo
		bool ActivatedCondition(std::string moduleName, std::string conditionKey);

		// Obtiene un valor numérico/bool de los settings de un módulo
		json GetValue(std::string moduleName, std::string settingKey);
	};
}
