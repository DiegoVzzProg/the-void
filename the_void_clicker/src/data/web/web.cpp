#include "web.h"


namespace Online {
	void CWeb::Init() {
        ix::initNetSystem();

        std::string url = "ws://127.0.0.1:5010/ws";
        clientSocket.setUrl(url);
        clientSocket.enableAutomaticReconnection();

        // Configuración para Localhost (sin SSL estricto)
        //ix::SocketTLSOptions tlsOptions;
        //tlsOptions.caFile = "NONE";
        //tlsOptions.disable_hostname_validation = true;
        //clientSocket.setTLSOptions(tlsOptions);

        // Headers de Identificación (Para pasar el filtro de wsManager.js)
        ix::WebSocketHttpHeaders headers;
        headers["x-hwid"] = "GHOST_CLIENT_01";
        clientSocket.setExtraHeaders(headers);

        DEBUG_LOG("Iniciando socket...");

        clientSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
            if (msg->type == ix::WebSocketMessageType::Open) {
                DEBUG_LOG(">> CONECTADO A LA NUBE <<");
            }
            else if (msg->type == ix::WebSocketMessageType::Error) {
                DEBUG_LOG("Error de Conexion: " << msg->errorInfo.reason);
            }
            else if (msg->type == ix::WebSocketMessageType::Close) {
                DEBUG_LOG("Desconectado.Codigo: " << msg->closeInfo.code);
            }
            else if (msg->type == ix::WebSocketMessageType::Message) {
                DEBUG_LOG("RECIBIO MENSAJE: " << msg->str);
                try {
                    auto data = json::parse(msg->str);

                    std::lock_guard<std::mutex> lock(candado);

                    module_manager_value = data;
                    is_new_command = true;
                }
                catch (json::parse_error& e) {
                    DEBUG_LOG(" Error parseando JSON del servidor: " << e.what());
                }
            }
            });

        clientSocket.start();
	}

    void CWeb::CleanUp() {
        DEBUG_LOG("Deteniendo servicios de red...");
        clientSocket.stop();
        ix::uninitNetSystem();
    }

    bool CWeb::ModuleActivated(std::string key) {
        std::lock_guard<std::mutex> lock(candado);

        if (!is_new_command && module_manager_value.is_null()) {
            return false;
        }

        std::string targetModule = module_manager_value[xorstr("module")];

        if (targetModule != key) {
            return false;
        }
    }

    auto CWeb::GetModuleValue(std::string key) {
        return module_manager_value[key];
    }
}