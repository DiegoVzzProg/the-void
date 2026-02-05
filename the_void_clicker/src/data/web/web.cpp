#include "web.h"


namespace Online {
    void CWeb::Init() {
        ix::initNetSystem();

        std::string url = xorstr("ws://127.0.0.1:5010/ws");
        clientSocket.setUrl(url);
        clientSocket.enableAutomaticReconnection();

        // Headers
        ix::WebSocketHttpHeaders headers;
        headers[xorstr("x-hwid")] = xorstr("GHOST_CLIENT_01");
        clientSocket.setExtraHeaders(headers);

        LOG_INFO(xorstr("Iniciando socket..."));

        clientSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {

            if (msg->type == ix::WebSocketMessageType::Open) {
                LOG_INFO(xorstr(">> CONECTADO A LA NUBE <<"));
            }
            else if (msg->type == ix::WebSocketMessageType::Error) {
                std::string error = xorstr("Error de Conexion: ") + msg->errorInfo.reason;
                LOG_ERROR(error);
            }
            else if (msg->type == ix::WebSocketMessageType::Close) {
                std::string warn = xorstr("Desconectado Codigo: ") + std::to_string(msg->closeInfo.code);
                LOG_WARN(warn);
            }
            else if (msg->type == ix::WebSocketMessageType::Message) {
                std::string success = xorstr("RECIBIO MENSAJE: ") + msg->str;
                LOG_INFO(success);
                try {
                    auto data = json::parse(msg->str);

                    std::lock_guard<std::mutex> lock(candado);
                    // --- LOGICA DE CACHEO ---
                    std::string moduleName = data["module"].get<std::string>();

                    module_cache[moduleName] = data;

                    if (data.contains("conditions")) {
                        cached_conditions.clear();
                        for (auto& item : data["conditions"]) {
                            cached_conditions.push_back(item.get<std::string>());
                        }
                    }

                    is_new_command = true;
                }
                catch (json::parse_error& e) {
                    std::string error_message = xorstr(" Error parseando JSON del servidor: ") + std::string(e.what());
                    LOG_ERROR(error_message);
                }
            }
            });

        clientSocket.start();
    }

    void CWeb::CleanUp() {
        clientSocket.stop();
        ix::uninitNetSystem();
    }


    bool CWeb::IsModuleActive(std::string moduleName) {
        std::lock_guard<std::mutex> lock(candado);

        if (module_cache.find(moduleName) == module_cache.end()) {
            return false;
        }

        try {
            return module_cache[moduleName][xorstr("action")] == xorstr("enable");
        }
        catch (...) {
            return false;
        }
    }

    // Obtiene un valor específico de los settings del módulo
    json CWeb::GetValue(std::string moduleName, std::string settingKey) {
        std::lock_guard<std::mutex> lock(candado);

        // 1. Validar existencia del módulo
        if (module_cache.find(moduleName) == module_cache.end()) return 0;

        try {
            auto& moduleData = module_cache[moduleName];

            // 2. Validar existencia de settings
            if (!moduleData.contains(xorstr("settings"))) return 0;

            // 3. Validar existencia de la llave específica (ej. "cps")
            if (moduleData[xorstr("settings")].contains(settingKey)) {
                return moduleData[xorstr("settings")][settingKey];
            }
        }
        catch (...) {
            return 0;
        }

        return 0; // Valor por defecto si falla algo
    }
    bool CWeb::ActivatedCondition(std::string moduleName, std::string conditionKey) {
        std::lock_guard<std::mutex> lock(candado);

        if (module_cache.find(moduleName) == module_cache.end()) return false;

        try {
            auto& moduleData = module_cache[moduleName];

            if (moduleData.contains(xorstr("conditions"))) {
                for (auto& item : moduleData[xorstr("conditions")]) {
                    if (item.get<std::string>() == conditionKey) return true;
                }
            }
        }
        catch (...) {
            return false;
        }

        return false;
    }
}