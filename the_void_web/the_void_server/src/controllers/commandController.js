const { sendCommandToClient } = require("../websocket/wsManager");
const commandService = require("../services/commandService");
const TokenManager = require("../utils/tokenManager");

const CommandController = {
  processCommand: async (req, reply) => {
    try {
      const command = req.body;
      const authHeader = req.headers.authorization;
      if (!authHeader) {
        throw new Error("No authorization header provided");
      }
      const token = authHeader.replace("Bearer ", "");
      const decoded = TokenManager.verifyAccessToken(token);
      const userId = decoded.id;

      const sent = sendCommandToClient(command);

      if (sent) {
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

  loadConfig: async (req, reply) => {
    try {
      const command = req.body;
      const authHeader = req.headers.authorization;

      if (!authHeader) {
        throw new Error("No authorization header provided");
      }
      const data = commandService.loadConfig(authHeader);

      return data;
    } catch (error) {
      console.error(error); // Es bueno loguear el error para verlo en consola
      return reply.code(400).send({
        error: "command failed",
        message: error.message,
      });
    }
  },
};

module.exports = CommandController;
