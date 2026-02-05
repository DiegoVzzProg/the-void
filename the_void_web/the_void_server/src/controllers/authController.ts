import { FastifyRequest, FastifyReply } from 'fastify';
import authService from "../services/authService";

const authController = {
  register: async (req: FastifyRequest, reply: FastifyReply) => {
    // We expect body to match RegisterParams, can validate or cast
    const result = await authService.registerUser(req.body as any); 
    return reply.code(201).send(result);
  },

  login: async (req: FastifyRequest, reply: FastifyReply) => {
    const currentHwid = req.headers["x-hwid"];
    // Cast hwid to string | undefined. Headers are string | string[] | undefined
    const hwid = Array.isArray(currentHwid) ? currentHwid[0] : currentHwid;
    
    const result = await authService.loginUser(req.body as any, hwid);
    return reply.code(200).send(result);
  },

  refreshToken: async (req: FastifyRequest, reply: FastifyReply) => {
    const result = await authService.refreshToken(req.body as any);
    return reply.code(200).send(result);
  },
};

export default authController;
