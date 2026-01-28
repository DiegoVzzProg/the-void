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
                    module_manager_value = data;

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

    bool CWeb::ModuleActivated() {
        std::lock_guard<std::mutex> lock(candado);
        return module_manager_value[xorstr("action")] == xorstr("enable");
    }

    bool CWeb::ItIsTheModule(std::string key) {
        std::lock_guard<std::mutex> lock(candado);

        if (!is_new_command && module_manager_value.is_null()) {
            return false;
        }

        std::string targetModule = module_manager_value[xorstr("module")];

        if (targetModule != key) {
            return false;
        }

        return true;
    }

    json CWeb::GetModuleValue(std::string key) {
        return module_manager_value[key];
    }

    json CWeb::GetValue(std::string key) {
        // 1. Seguridad: Si el JSON global está vacío o nulo, devolvemos 0 (o null) para no crashear.
        if (module_manager_value.is_null()) {
            return 0;
        }

        // 2. Seguridad: Verificamos si existe la sección "settings"
        if (!module_manager_value.contains(xorstr("settings"))) {
            return 0;
        }

        auto& settings = module_manager_value[xorstr("settings")];

        // 3. Dinamismo: Buscamos la llave que TÚ pediste (key), no "cps".
        if (settings.contains(key)) {
            return settings[key]; // Devuelve el objeto json (int, bool, float, etc.)
        }

        // Si no existe la configuración, devolvemos 0 por defecto.
        return 0;
    }

    void CWeb::AcknowledgeCommand() {
        std::lock_guard<std::mutex> lock(candado);
        is_new_command = false;
        module_manager_value = nullptr; 
    }

    bool CWeb::ActivatedCondition(std::string key) {
        std::lock_guard<std::mutex> lock(candado);
        for (const auto& cond : cached_conditions) {
            if (cond == key) return true;
        }
        return false;
    }
}