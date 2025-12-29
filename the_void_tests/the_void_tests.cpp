#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXNetSystem.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>  // [1] Necesario para evitar conflictos entre hilos

using json = nlohmann::json;

int main() {
    ix::initNetSystem();

    // --- VARIABLES COMPARTIDAS ---
    // Estas variables vivirán fuera del callback para que el main las vea.
    json ultimoComandoJson;          // Aquí guardaremos el JSON procesado
    bool hayNuevoComando = false;    // Una bandera para saber si llegó algo nuevo
    std::mutex candado;              // El "semáforo" de seguridad
    // -----------------------------

    ix::WebSocket webSocket;
    webSocket.setUrl("ws://localhost:5010/ws");
    webSocket.enableAutomaticReconnection();

    // Configuraciones TLS (para localhost)
    ix::SocketTLSOptions tlsOptions;
    tlsOptions.caFile = "NONE";
    tlsOptions.disable_hostname_validation = true;
    webSocket.setTLSOptions(tlsOptions);

    std::cout << "[C++] Cliente iniciado..." << std::endl;

    // Pasamos las variables por referencia [&] para poder escribirlas
    webSocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {

            // 1. Intentamos convertir el texto a JSON
            try {
                auto dataParseada = json::parse(msg->str);

                // 2. BLOQUEAMOS el acceso (Ponemos el candado)
                std::lock_guard<std::mutex> lock(candado);

                // 3. Escribimos los datos de forma segura
                ultimoComandoJson = dataParseada;
                hayNuevoComando = true;

                // El candado se libera solo al terminar este bloque
            }
            catch (json::parse_error& e) {
                std::cerr << "[Error] El mensaje no es un JSON valido: " << e.what() << std::endl;
            }
        }
        });

    webSocket.start();

    // --- TU BUCLE PRINCIPAL ---
    while (true) {

        // Creamos un bloque { } pequeño para bloquear y desbloquear rápido
        {
            std::lock_guard<std::mutex> lock(candado); // Ponemos candado para leer

            if (hayNuevoComando) {
                // AQUI USAS TU JSON
                std::cout << "--- PROCESANDO EN BUCLE PRINCIPAL ---" << std::endl;

                // Ejemplo: Leer valores del JSON
                // Supongamos que recibes: { "accion": "disparar", "fuerza": 100 }

                if (ultimoComandoJson.contains("accion")) {
                    std::string accion = ultimoComandoJson["accion"];
                    std::cout << "Ejecutando accion: " << accion << std::endl;

                    if (accion == "salir") break; // Ejemplo para romper el bucle
                }

                if (ultimoComandoJson.contains("fuerza")) {
                    int fuerza = ultimoComandoJson["fuerza"];
                    std::cout << "Fuerza aplicada: " << fuerza << std::endl;
                }

                // Reseteamos la bandera para no procesar lo mismo 2 veces
                hayNuevoComando = false;
            }
        } // Aquí se libera el candado automáticamente

        std::cout.flush();
        // Dormimos un poco para no quemar la CPU (ej. 33ms = ~30 FPS)
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    ix::uninitNetSystem();
    return 0;
}