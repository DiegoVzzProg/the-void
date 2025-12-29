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

    std::string url = "ws://localhost:5010/ws";
    clientSocket.setUrl(url);
    clientSocket.enableAutomaticReconnection();

    // Configuración para Localhost (sin SSL estricto)
    ix::SocketTLSOptions tlsOptions;
    tlsOptions.caFile = "NONE";
    tlsOptions.disable_hostname_validation = true;
    clientSocket.setTLSOptions(tlsOptions);

    std::cout << "[WEB] Iniciando socket..." << std::endl;

    clientSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "[DLL] >> CONECTADO A LA NUBE <<" << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Message) {
            std::cout << "[DLL] RAW: " << msg->str << std::endl;
            try {
                auto data = json::parse(msg->str);

                std::lock_guard<std::mutex> lock(Web::candado);
                Web::module_manager_value = data;
                Web::is_new_command = true;
            }
            catch (...) {
                std::cout << "[DLL] Error parseando JSON" << std::endl;
            }
        }
        });

    clientSocket.start();
}

void Web::Cleanup() {
    clientSocket.stop();
    ix::uninitNetSystem();
}