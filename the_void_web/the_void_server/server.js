require("dotenv").config();
const fastify = require("fastify")({ logger: true }); // Logger activado
const { handleConnection } = require("./src/websocket/wsManager");

const api = require("./src/routes/api");

const start = async () => {
  try {
    await fastify.register(require("@fastify/cors"), { origin: true });
    await fastify.register(require("@fastify/websocket"));

    await fastify.register(api, { prefix: "/api" });

    fastify.register(async function (fastify) {
      fastify.get("/ws", { websocket: true }, handleConnection);
    });

    const PORT = process.env.PORT || 5010;
    await fastify.listen({ port: PORT, host: "0.0.0.0" });

    console.log(`
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
