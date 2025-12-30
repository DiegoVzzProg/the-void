#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXNetSystem.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

using json = nlohmann::json;

int main() {
    // 1. Inicializar sistema de red (Obligatorio en Windows)
    ix::initNetSystem();

    // Variables compartidas
    json ultimoComandoJson;
    bool hayNuevoComando = false;
    std::mutex candado;

    ix::WebSocket webSocket;

    // Conexión a Localhost explícito
    webSocket.setUrl("ws://127.0.0.1:5010/ws");

    // Auto-reconectarse si el servidor se reinicia
    webSocket.enableAutomaticReconnection();

    // CAMBIO: ELIMINADAS LAS OPCIONES TLS (tlsOptions). 
    // No son necesarias para ws:// y causan conflictos.

    // Headers de identificación
    ix::WebSocketHttpHeaders headers;
    headers["x-hwid"] = "GHOST_CLIENT_01";
    headers["Authorization"] = "Bearer LICENCIA_FALSA_123";
    webSocket.setExtraHeaders(headers);

    std::cout << "[C++] Iniciando cliente..." << std::endl;

    webSocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "[EXITO] Conectado al Backend!" << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Error) {
            std::cerr << "[ERROR] Fallo de conexion: " << msg->errorInfo.reason << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Close) {
            std::cout << "[INFO] Desconectado. Codigo: " << msg->closeInfo.code << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Message) {
            // Procesar Mensaje
            try {
                auto dataParseada = json::parse(msg->str);

                // Bloquear y guardar
                std::lock_guard<std::mutex> lock(candado);
                ultimoComandoJson = dataParseada;
                hayNuevoComando = true;

                // 1. Obtener nombre del módulo
                std::string modulo = ultimoComandoJson["module"].get<std::string>();
                std::string accion = ultimoComandoJson["action"].get<std::string>();

                // 2. Obtener Settings (si existen)
                if (ultimoComandoJson.contains("settings")) {
                    auto& settings = ultimoComandoJson["settings"];

                    if (modulo == "reach") {
                        // Solo buscamos lo que le importa a Reach
                        if (settings.contains("range")) {
                            double rng = settings["range"];
                            // ReachModule::setRange(rng);
                        }
                        if (settings.contains("hitchance")) {
                            double hc = settings["hitchance"];
                            // ReachModule::setChance(hc);
                        }
                    }
                    else if (modulo == "autoclicker") {
                        // Solo buscamos lo que le importa a AC
                        if (settings.contains("cps")) {
                            int cps = settings["cps"];
                            // ClickerModule::setCPS(cps);
                        }
                    }
                }

                // Debug rápido en consola
                std::cout << "[RECV] JSON recibido del servidor" << std::endl;
            }
            catch (json::parse_error& e) {
                std::cerr << "[JSON Error] " << e.what() << std::endl;
            }
        }
        });

    webSocket.start();

    // Bucle Principal
    while (true) {
        {
            std::lock_guard<std::mutex> lock(candado);
            if (hayNuevoComando) {
                std::cout << "\n--- COMANDO RECIBIDO ---" << std::endl;

                if (ultimoComandoJson.contains("module")) {
                    std::string modulo = ultimoComandoJson["module"];
                    std::string accion = ultimoComandoJson["action"];
                    std::cout << " > Ejecutando: " << modulo << " -> " << accion << std::endl;
                }
                hayNuevoComando = false;
            }
        }

        // 50ms de sleep (~20 ticks por segundo, ligero para la CPU)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    ix::uninitNetSystem();
    return 0;
}