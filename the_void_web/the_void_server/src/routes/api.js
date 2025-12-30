const authController = require("../controllers/authController");
const authSchema = require("../schemas/authSchema");

const commandController = require("../controllers/commandController");
const commandSchema = require("../schemas/commandSchema");

const api = async (fastify, options) => {
  fastify.post(
    "/command",
    { schema: commandSchema },
    commandController.processCommand
  );

  fastify.post(
    "/auth/register",
    { schema: authSchema.register },
    authController.register
  );
  fastify.post(
    "/auth/login",
    { schema: authSchema.login },
    authController.login
  );
};

module.exports = api;
