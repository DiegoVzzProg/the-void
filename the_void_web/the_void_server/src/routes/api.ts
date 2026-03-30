import { FastifyInstance } from 'fastify';
import { AuthRoutes } from '@/modules/auth';
import { CommandRoutes } from '@/modules/commands';

const api = async (fastify: FastifyInstance) => {
  fastify.get('/', async (request, reply) => {
    return { message: 'Welcome to The Void API' };
  });

  AuthRoutes.Routes(fastify);
  CommandRoutes.Routes(fastify);
};

export default api;