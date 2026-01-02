#include "web.h"
#include <iostream>
#include <thread>
#include <chrono>

// IMPORTANTE: Definir esto antes de incluir ixwebsocket si usas static linking
// (Aunque es mejor ponerlo en las propiedades del proyecto como dije en el Paso 2)
#ifndef IXWEBSOCKET_STATIC
#define IXWEBSOCKET_STATIC
#endif

#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXNetSystem.h>

// Variable GLOBAL (Esta mantiene la conexión viva)
static ix::WebSocket clientSocket;

void Web::Init() {
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
    headers["x-hwid"] = "GHOST_CLIENT_01"; // Debe coincidir con lo que espera tu Backend
    clientSocket.setExtraHeaders(headers);

    std::cout << "[WEB] Iniciando socket..." << std::endl;

    clientSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "[DLL] >> CONECTADO A LA NUBE <<" << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Error) {
            // Esto te dirá si falla por Firewall o Puerto cerrado
            std::cerr << "[DLL CRITICAL] Error de Conexion: " << msg->errorInfo.reason << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Close) {
            std::cout << "[DLL] Desconectado. Codigo: " << msg->closeInfo.code << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Message) {
            std::cout << "[DLL] RECIBIO MENSAJE: " << msg->str << std::endl;
            try {
                auto data = json::parse(msg->str);

                // --- ZONA CRÍTICA (THREAD SAFETY) ---
                // Bloqueamos el mutex para escribir en la memoria compartida
                // Esto evita que el juego crashee si intenta leer mientras escribimos
                std::lock_guard<std::mutex> lock(Web::candado);

                Web::module_manager_value = data;
                Web::is_new_command = true;
                // El candado se libera aquí automáticamente
            }
            catch (json::parse_error& e) {
                std::cerr << "[DLL] Error parseando JSON del servidor: " << e.what() << std::endl;
            }
        }
        });

    clientSocket.start();
}

void Web::Cleanup() {
    std::cout << "[DLL] Deteniendo servicios de red..." << std::endl;
    clientSocket.stop();
    ix::uninitNetSystem();
}

bool Web::ActivatedCondition(std::string key) {
    auto& conditions = Web::module_manager_value["conditions"];

    bool exists = false;

    if (conditions.empty()) {
        return exists;
    }

    for (auto& condition : conditions) {
        std::string condStr = condition.get<std::string>();

        if (condStr == key)
        {
            exists = true;
            break;
        }
        else
        {
            exists = false;
            break;
        }
    }

    return exists;
}