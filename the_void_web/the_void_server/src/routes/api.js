const authController = require("../controllers/authController");
const authSchema = require("../schemas/authSchema");

const commandController = require("../controllers/commandController");
const commandSchema = require("../schemas/commandSchema");

const authenticate = require("../middlewares/authMiddleware");
const api = async (fastify, options) => {
  fastify.post(
    "/command",
    { schema: commandSchema, onRequest: [authenticate] },
    commandController.processCommand
  );

  fastify.get(
    "/command/load-config",
    { onRequest: [authenticate] },
    commandController.loadConfig
  );

  fastify.post(
    "/auth/register",
    {
      schema: authSchema.register,
      config: {
        rateLimit: {
          max: 5,
          timeWindow: "1 hour",
        },
      },
    },
    authController.register
  );

  fastify.post(
    "/auth/login",
    {
      schema: authSchema.login,
      config: {
        rateLimit: {
          max: 5,
          timeWindow: "1 hour",
        },
      },
    },
    authController.login
  );

  fastify.post(
    "/auth/refresh-token",
    {
      schema: authSchema.refresh_token,
      config: {
        rateLimit: {
          max: 3,
          timeWindow: "1 hour",
        },
      },
    },
    authController.refreshToken
  );
};

module.exports = api;
