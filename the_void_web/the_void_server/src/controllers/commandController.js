const { sendCommandToClient } = require("../websocket/wsManager");
const commandService = require("../services/commandService");
const TokenManager = require("../utils/tokenManager");

const CommandController = {
  processCommand: async (req, reply) => {
    try {
      const command = req.body;

      // 1. Extraemos el usuario ANTES de llamar al servicio
      // Esto es necesario para obtener el 'id' que pide Prisma
      const authHeader = req.headers.authorization;
      if (!authHeader) {
        throw new Error("No authorization header provided");
      }
      const token = authHeader.replace("Bearer ", "");
      const decoded = TokenManager.verifyAccessToken(token);
      const userId = decoded.id;

      const sent = sendCommandToClient(command);

      if (sent) {
        // 2. CORRECCIÓN AQUÍ:
        // Pasamos 'command' (que es req.body) y el 'userId'
        await commandService.saveConfig(command, userId);

        return { status: "Connected" };
      } else {
        reply.code(503);
        throw new Error("Disconnected");
      }
    } catch (error) {
      console.error(error); // Es bueno loguear el error para verlo en consola
      return reply.code(400).send({
        error: "command failed",
        message: error.message,
      });
    }
  },

  saveConfig: async (req, reply) => {
    try {
      const authHeader = req.headers.authorization;
      const token = authHeader.replace("Bearer ", "");
      const decoded = TokenManager.verifyAccessToken(token);

      await commandService.saveConfig(req.body, decoded.id);
      return reply.code(200).send({
        status: "Connected",
      });
    } catch (error) {
      return reply.code(400).send({
        error: "the configuration was not saved",
        message: error.message,
      });
    }
  },
};

module.exports = CommandController;
