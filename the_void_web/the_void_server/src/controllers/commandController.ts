import { FastifyRequest, FastifyReply } from 'fastify';
import commandService from "../services/commandService";
// 1. IMPORTAR EL GESTOR DE WEBSOCKET
import { sendCommandToClient } from "../websocket/wsManager"; 

const commandController = {
  processCommand: async (req: FastifyRequest, reply: FastifyReply) => {
     const user = (req as any).user;
     
     if (!user) return reply.code(401).send({error: "Unauthorized"});
     
     // Guardamos en DB (Esto ya lo hacías)
     const result = await commandService.saveConfig(req.body, user.id);

     // 2. [FIX CRÍTICO] ENVIAR AL CLIENTE C++
     // Aquí es donde "empujamos" el JSON hacia el socket abierto
     const sent = sendCommandToClient(req.body); 

     if (sent) {
         console.log("[API] Comando enviado al Socket exitosamente.");
     } else {
         console.warn("[API] Se guardó config pero el cliente C++ no parece conectado.");
     }

     return reply.send(result);
  },

  loadConfig: async (req: FastifyRequest, reply: FastifyReply) => {
      const authHeader = req.headers.authorization;
      if (!authHeader) return reply.code(401).send({error: "Missing token"});

      const result = await commandService.loadConfig(authHeader);
      return reply.send(result);
  }
};

export default commandController;