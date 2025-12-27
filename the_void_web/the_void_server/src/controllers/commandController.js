const { sendCommand } = require("../websocket/wsManager");

const processCommand = (req, res) => {
  const command = req.body; // { Module: "Reach", Enabled: true, ... }

  console.log(`[API] Comando recibido: ${JSON.stringify(command)}`);

  // Intentar enviar al C++
  const sent = sendCommand(command);

  if (sent) {
    res.status(200).json({ status: "Enviado al C++ con éxito" });
  } else {
    res.status(503).json({ error: "El Cliente C++ no está conectado" });
  }
};

module.exports = { processCommand };
