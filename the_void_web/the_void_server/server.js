require("dotenv").config();
const express = require("express");
const cors = require("cors");
const http = require("http");
const { setupWebSocket } = require("./src/websocket/wsManager");
const { processCommand } = require("./src/controllers/commandController");

const app = express();
const server = http.createServer(app); // Creamos servidor HTTP crudo para atarle WS

// Middlewares
app.use(cors()); // Permite conexiones desde Vue
app.use(express.json()); // Entiende JSON en el body

// Rutas HTTP (API)
app.post("/api/command", processCommand);
app.get("/", (req, res) => res.send("The Void Backend is Running..."));

// Inicializar WebSockets
setupWebSocket(server);

// Arrancar
const PORT = process.env.PORT || 5010;
server.listen(PORT, () => {
  console.log(`==========================================`);
  console.log(`   THE VOID - BACKEND OPERATIVO`);
  console.log(`   HTTP: http://localhost:${PORT}`);
  console.log(`   WSS:  ws://localhost:${PORT}${process.env.WS_PATH || "/ws"}`);
  console.log(`==========================================`);
});
