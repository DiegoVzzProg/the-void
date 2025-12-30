// src/websocket/wsManager.js
const clients = new Map();

function handleConnection(connection, req) {
  try {
    // --- CORRECCIÓN CRÍTICA ---
    // A veces Fastify entrega 'connection' como un wrapper { socket: ... }
    // y otras veces entrega el socket directo. Aquí detectamos cuál es.
    let socket;

    if (connection.socket) {
      socket = connection.socket; // Es un wrapper (SocketStream)
    } else {
      socket = connection; // Es el socket directo (Raw WebSocket)
    }

    // Validación de seguridad para que no explote
    if (!socket || typeof socket.on !== "function") {
      console.error(
        "[WS CRITICAL] El objeto recibido no es un WebSocket válido:",
        connection
      );
      return; // Salimos para evitar el crash
    }
    // ---------------------------

    const clientId = "GHOST_CLIENT_01";

    console.log(
      `[WS DEBUG] Socket validado correctamente para IP: ${
        req ? req.ip : "Desconocida"
      }`
    );

    clients.set(clientId, socket);
    console.log(`[WS] Cliente registrado en memoria: ${clientId}`);

    // Manejador de Errores (Vital para estabilidad)
    socket.on("error", (err) => console.error(`[WS ERROR] ${err.message}`));

    socket.on("message", (msg) => {
      console.log(`[WS RECV] Mensaje del cliente: ${msg.toString()}`);
    });

    socket.on("close", () => {
      console.log(`[WS] Cliente desconectado: ${clientId}`);
      clients.delete(clientId);
    });

    // Enviar bienvenida
    // Usamos try-catch aquí también por si el socket no está listo
    try {
      socket.send(JSON.stringify({ type: "WELCOME", msg: "Backend Listo" }));
    } catch (sendErr) {
      console.error("[WS WARN] No se pudo enviar bienvenida:", sendErr.message);
    }
  } catch (err) {
    console.error("[WS FATAL] Error general en handleConnection:", err);
  }
}

function sendCommandToClient(commandData) {
  // Debug para ver si Postman funciona
  console.log(
    "[WS DEBUG] Intentando enviar comando. Clientes activos:",
    Array.from(clients.keys())
  );

  const socket = clients.get("GHOST_CLIENT_01");

  if (socket && socket.readyState === 1) {
    socket.send(JSON.stringify(commandData));
    return true;
  }

  console.log("[WS WARN] Cliente no encontrado o socket cerrado.");
  return false;
}

module.exports = { handleConnection, sendCommandToClient };
