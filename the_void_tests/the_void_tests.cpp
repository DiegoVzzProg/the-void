#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXNetSystem.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <iostream>

using json = nlohmann::json;

int main() {
    ix::initNetSystem();

    ix::WebSocket webSocket;

    std::string url = "ws://localhost:5010/ws";
    webSocket.setUrl(url);

    // [NUEVO] 2. Habilitar reconexión automática
    // Si la API se reinicia, el cliente intentará volver a conectarse solo.
    webSocket.enableAutomaticReconnection();

    // [NUEVO] 3. Opcional: Desactivar verificación de certificados para localhost
    // Esto evita desconexiones si estás probando en un entorno local raro.
    ix::SocketTLSOptions tlsOptions;
    tlsOptions.caFile = "NONE";
    tlsOptions.disable_hostname_validation = true;
    webSocket.setTLSOptions(tlsOptions);

    std::cout << "[C++] Iniciando 'The Void' Client..." << std::endl;

    webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            std::cout << "[C++] Comando Recibido: " << msg->str << std::endl;
            // ... tu lógica de parseo JSON ...
        }
        else if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "[C++] >> CONECTADO AL SERVIDOR <<" << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Close) {
            std::cout << "[C++] >> DESCONECTADO (Codigo: " << msg->closeInfo.code << ") <<" << std::endl;
        }
        else if (msg->type == ix::WebSocketMessageType::Error) {
            std::cout << "[C++] >> ERROR DE CONEXION: " << msg->errorInfo.reason << std::endl;
        }
        });

    webSocket.start();

    // Mantener vivo
    while (true) {
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    ix::uninitNetSystem();
    return 0;
}