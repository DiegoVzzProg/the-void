import { FastifyRequest, FastifyReply } from 'fastify';
import TokenManager from '../utils/tokenManager';

/**
 * Middleware para proteger rutas.
 * Verifica Token + HWID.
 */
async function authenticate(req: FastifyRequest, reply: FastifyReply) {
  try {
    // 1. Obtener el token del header "Authorization: Bearer <token>"
    const authHeader = req.headers.authorization;
    if (!authHeader) {
      return reply
        .code(401)
        .send({ error: "Unauthorized", message: "Missing token" });
    }

    const token = authHeader.replace("Bearer ", "");

    // 2. Verificar firma criptográfica
    const decoded = TokenManager.verifyAccessToken(token);
    if (!decoded) {
      return reply.code(401).send({ error: "Invalid or expired token" });
    }

    // 3. --- SEGURIDAD ANTI-ROBO (HWID BINDING) ---
    // El cliente C++ envía su HWID en el header 'x-hwid'
    const currentHwid = req.headers["x-hwid"];

    if (!currentHwid) {
      return reply.code(400).send({ error: "Missing HWID header" });
    }

    // Type guard / Check if decoded is object and has hwid
    if (typeof decoded === 'string' || !('hwid' in decoded)) {
       return reply.code(401).send({ error: "Invalid token payload" });
    }

    // Si el HWID del token no coincide con el de la petición actual...
    if (decoded.hwid !== currentHwid) {
      console.warn(
        `[ALERTA DE SEGURIDAD] Robo de Token detectado. User: ${decoded.id}`
      );
      // Bloqueamos la petición inmediatamente
      return reply
        .code(403)
        .send({ error: "Access denied: Token used on another machine" });
    }

    // 4. Todo correcto, inyectamos el usuario en la request
    req.user = decoded;
  } catch (err) {
    return reply.code(401).send({ error: "Authentication error" });
  }
}

export default authenticate;
