import { FastifyInstance, FastifyPluginOptions } from 'fastify';
import authController from "../controllers/authController";
import commandController from "../controllers/commandController";
import * as authSchema from "../schemas/authSchema";
import commandSchema from "../schemas/commandSchema";
import authenticate from "../middlewares/authMiddleware";

const api = async (fastify: FastifyInstance, options: FastifyPluginOptions) => {
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

export default api;
