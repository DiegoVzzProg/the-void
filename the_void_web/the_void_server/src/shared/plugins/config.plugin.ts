import { type FastifyInstance } from "fastify";
import cors from "@fastify/cors";
import { serializerCompiler, validatorCompiler } from "fastify-type-provider-zod";
import rateLimiterPlugin from "./rate-limiter.plugin";
import api from "@/routes/api";
import websocket from '@fastify/websocket';
import errorHandler from "@/shared/utils/error-handler";
import { WsManager } from "./ws-manager.plugin";


export const PORT = Number(process.env.PORT || 5010);
export const HOST = process.env.HOST || '0.0.0.0';
export const PREFIX = process.env.PREFIX || '/api';
export const WS_PREFIX = process.env.WS_PREFIX || '/ws';

const configPlugin = async (fastify: FastifyInstance) => {
    fastify.setErrorHandler(errorHandler);
    fastify.setValidatorCompiler(validatorCompiler);
    fastify.setSerializerCompiler(serializerCompiler);

    await fastify.register(cors, { origin: true });
    await fastify.register(websocket);
    await fastify.register(rateLimiterPlugin);

    await fastify.register(api, { prefix: PREFIX });


    fastify.register(async function (fastify) {
        fastify.get(WS_PREFIX, { websocket: true }, WsManager.HandleConnection);
    });
}

export default configPlugin;