import { FastifyRequest, FastifyReply } from 'fastify';
import { TokenManager } from '@/shared/utils/token-manager.utils';

/**
 * Middleware para proteger rutas.
 * Verifica Token + HWID.
 */
async function authenticate(req: FastifyRequest, reply: FastifyReply) {
  try {
    const authHeader = req.headers.authorization;
    if (!authHeader) {
      return reply
        .code(401)
        .send({ error: "Unauthorized", message: "Missing token" });
    }

    const token = authHeader.replace("Bearer ", "");

    const decoded = TokenManager.VerifyAccessToken(token);
    if (!decoded) {
      return reply.code(401).send({ error: "Invalid or expired token" });
    }

    req.user = decoded;
  } catch (err) {
    return reply.code(401).send({ error: "Authentication error" });
  }
}

export default authenticate;
