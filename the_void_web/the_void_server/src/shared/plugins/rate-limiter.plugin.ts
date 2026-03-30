import rateLimit from '@fastify/rate-limit';
import { type FastifyInstance } from 'fastify';

const rateLimiterPlugin = async (fastify: FastifyInstance) => {
    await fastify.register(rateLimit, {
        max: 100,
        timeWindow: "1 minute",
        errorResponseBuilder: function (_request, _context) {
            return {
                code: 429,
                error: "Too Many Requests",
                message: "You've gone too far. Slow down, cowboy.",
            };
        },
    });
}

export default rateLimiterPlugin;