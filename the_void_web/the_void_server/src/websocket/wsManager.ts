import { FastifyRequest } from 'fastify';
import WebSocket from 'ws';

const clients = new Map<string, WebSocket>();

export function handleConnection(connection: any, req: FastifyRequest) {
  try {
    // --- CORRECCIÓN CRÍTICA ---
    // A veces Fastify entrega 'connection' como un wrapper { socket: ... }
    // y otras veces entrega el socket directo. Aquí detectamos cuál es.
    // Fastify-websocket siempre entrega un objeto SocketStream en 'connection'
    // La propiedad .socket es el raw WebSocket de la librería 'ws'
    const socket: WebSocket = connection.socket || connection;

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
      `[WS DEBUG] Socket validado correctamente para IP: ${req ? req.ip : "Desconocida"
      }`
    );

    clients.set(clientId, socket);
    console.log(`[WS] Cliente registrado en memoria: ${clientId}`);

    // Manejador de Errores (Vital para estabilidad)
    socket.on("error", (err: Error) => console.error(`[WS ERROR] ${err.message}`));

    socket.on("message", (msg: any) => {
      console.log(`[WS RECV] Mensaje del cliente: ${msg.toString()}`);
    });

    socket.on("close", () => {
      console.log(`[WS] Cliente desconectado: ${clientId}`);
      clients.delete(clientId);
    });

    // // Enviar bienvenida
    // try {
    //   socket.send(JSON.stringify({ type: "WELCOME", msg: "Backend Listo" }));
    // } catch (sendErr: any) {
    //   console.error("[WS WARN] No se pudo enviar bienvenida:", sendErr.message);
    // }
  } catch (err) {
    console.error("[WS FATAL] Error general en handleConnection:", err);
  }
}

export function sendCommandToClient(commandData: any) {
  // Debug para ver si Postman funciona
  console.log(
    "[WS DEBUG] Intentando enviar comando. Clientes activos:",
    Array.from(clients.keys())
  );

  const socket = clients.get("GHOST_CLIENT_01");

  if (socket && socket.readyState === WebSocket.OPEN) { // 1 = OPEN
    socket.send(JSON.stringify(commandData));
    return true;
  }

  console.log("[WS WARN] Cliente no encontrado o socket cerrado.");
  return false;
}
