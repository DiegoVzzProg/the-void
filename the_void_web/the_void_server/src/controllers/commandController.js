const { sendCommandToClient } = require("../websocket/wsManager");

const processCommand = async (req, reply) => {
  const command = req.body;

  const sent = sendCommandToClient(command);

  if (sent) {
    return { status: "Enviado al C++ con éxito" };
  } else {
    reply.code(503);
    throw new Error("El Cliente C++ no está conectado");
  }
};

module.exports = { processCommand };
