import dotenv from 'dotenv';
import Fastify from 'fastify';
import configPlugin, { PORT } from '@/shared/plugins/config.plugin';
import { ZodTypeProvider } from 'fastify-type-provider-zod';

dotenv.config();

const fastify = Fastify({ logger: true }).withTypeProvider<ZodTypeProvider>();;

const start = async () => {
  try {
    await fastify.register(configPlugin);

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
