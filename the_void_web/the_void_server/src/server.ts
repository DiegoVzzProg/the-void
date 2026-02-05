import dotenv from 'dotenv';
import Fastify from 'fastify';
import cors from '@fastify/cors';
import websocket from '@fastify/websocket';
import rateLimit from '@fastify/rate-limit';
import { handleConnection } from './websocket/wsManager';
import api from './routes/api';
import errorHandler from './utils/errorHandler';

dotenv.config();

const fastify = Fastify({ logger: true });

const start = async () => {
  try {
    await fastify.register(cors, { origin: true });
    await fastify.register(websocket);
    await fastify.register(rateLimit, {
      max: 100, // Máximo 100 peticiones globales
      timeWindow: "1 minute", // por minuto por IP
      errorResponseBuilder: function (request, context) {
        return {
          code: 429,
          error: "Too Many Requests",
          message: "You've gone too far. Slow down, cowboy.",
        };
      },
    });

    await fastify.register(api, { prefix: "/api" });

    // Global Error Handler
    fastify.setErrorHandler(errorHandler);

    fastify.register(async function (fastify) {
      fastify.get("/ws", { websocket: true }, handleConnection);
    });

    const PORT = process.env.PORT || 5010;
    await fastify.listen({ port: Number(PORT), host: "0.0.0.0" });

    fastify.log.info(`
      ====================================
      THE VOID - BACKEND
      API: http://127.0.0.1:${PORT}/api
      WSS: ws://127.0.0.1:${PORT}/ws
      ====================================
    `);
  } catch (err) {
    fastify.log.error(err);
    process.exit(1);
  }
};

start();
