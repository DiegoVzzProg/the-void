const WebSocket = require("ws");

let clientSocket = null;

const setupWebSocket = (server) => {
  const wss = new WebSocket.Server({
    server,
    path: process.env.WS_PATH || "/ws",
  });

  wss.on("connection", (ws) => {
    console.log('[WS] Cliente C++ Conectado ("The Void" activo)');
    clientSocket = ws;

    // Enviar Ping de bienvenida (opcional)
    ws.send(JSON.stringify({ type: "Welcome", msg: "Conectado al Backend" }));

    ws.on("close", () => {
      console.log("[WS] Cliente C++ Desconectado");
      clientSocket = null;
    });

    ws.on("error", (err) => console.error("[WS] Error:", err.message));
  });

  return wss;
};

// Función para enviar comandos desde la API hacia el C++
const sendCommand = (commandData) => {
  if (clientSocket && clientSocket.readyState === WebSocket.OPEN) {
    clientSocket.send(JSON.stringify(commandData));
    return true;
  }
  return false;
};

module.exports = { setupWebSocket, sendCommand };
