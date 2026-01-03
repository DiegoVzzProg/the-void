const { sendCommandToClient } = require("../websocket/wsManager");
const commandService = require("../services/commandService");

const CommandController = {
  processCommand: async (req, reply) => {
    try {
      const command = req.body;

      const sent = sendCommandToClient(command);

      if (sent) {
        return { status: "Connected" };
      } else {
        reply.code(503);
        throw new Error("Disconnected");
      }
    } catch (error) {
      return reply.code(400).send({
        error: "command failed",
        message: error.message,
      });
    }
  },
  saveConfig: async (req, reply) => {
    try {
      await commandService.saveConfig(req.body);
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
