import jwt from 'jsonwebtoken';
import { FastifyRequest } from "fastify";
import WebSocket from 'ws';
import { ITokenPayload } from '@/modules/auth';
import prisma from '@/shared/database/prisma';

interface UserSession {
    cppSocket?: WebSocket;
    webSocket?: WebSocket;
}

const sessions = new Map<string, UserSession>();

export class WsManager {
    public static async HandleConnection(connection: any, req: FastifyRequest) {
        try {
            const socket: WebSocket = connection.socket || connection;

            if (!socket || typeof socket.on !== "function") {
                console.error("[WS CRITICAL] Objeto no válido:", connection);
                return;
            }

            let userId = "";
            let clientType = "";
            const hwid = req.headers['x-hwid'];
            const query = req.query as { token?: string };

            if (hwid) {
                userId = hwid as string;
                clientType = "NATIVE_CPP";

                const user = await prisma.void_users.findUnique({ where: { hwid: userId } });
                if (!user || user.banned) {
                    console.warn(`[WS SECURITY] Intento de conexión C++ con HWID inválido o baneado: ${userId}`);
                    socket.close();
                    return;
                }
            } else if (query && query.token) {
                try {
                    const decoded = jwt.verify(query.token, process.env.JWT_ACCESS_SECRET as string) as ITokenPayload;
                    userId = decoded.hwid;
                    clientType = "WEB_DASHBOARD";
                } catch (e) {
                    console.warn("[WS SECURITY] Token inválido en conexión Web.");
                    socket.close();
                    return;
                }
            } else {
                console.warn("[WS SECURITY] Conexión rechazada: Sin identificación.");
                socket.close();
                return;
            }

            console.log(`[WS LOGIN] Tipo: ${clientType} | Usuario: ${userId}`);

            if (!sessions.has(userId)) {
                sessions.set(userId, {});
            }

            const userSession = sessions.get(userId)!;

            if (clientType === "NATIVE_CPP") {
                userSession.cppSocket = socket;
            } else {
                userSession.webSocket = socket;
            }

            socket.on("message", (rawMsg: any) => {
                const msgStr = rawMsg.toString();
                console.log(`[WS RECV] [${clientType}] -> ${msgStr}`);

                if (clientType === "WEB_DASHBOARD" && userSession.cppSocket?.readyState === WebSocket.OPEN) {
                    console.log(`[WS BRIDGE] Reenviando comando al Cliente C++...`);
                    userSession.cppSocket.send(msgStr);
                }

                if (clientType === "NATIVE_CPP" && userSession.webSocket?.readyState === WebSocket.OPEN) {
                    userSession.webSocket.send(msgStr);
                }
            });
            socket.on("close", () => {
                console.log(`[WS CLOSE] ${clientType} desconectado: ${userId}`);
                if (clientType === "NATIVE_CPP") userSession.cppSocket = undefined;
                else userSession.webSocket = undefined;

                if (!userSession.cppSocket && !userSession.webSocket) {
                    sessions.delete(userId);
                }
            });

            socket.on("error", (err: Error) => console.error(`[WS ERROR] ${err.message}`));

        } catch (err) {
            console.error("[WS FATAL] Error en handleConnection:", err);
        }
    }

    public static async SendCommandToClient(targetUserId: string, commandData: any) {
        const session = sessions.get(targetUserId);

        if (session?.cppSocket?.readyState === WebSocket.OPEN) {
            session.cppSocket.send(JSON.stringify(commandData));
            console.log(`[WS SEND] Comando enviado a C++ de ${targetUserId}`);
            return true;
        }

        if (session?.webSocket?.readyState === WebSocket.OPEN) {
            session.webSocket.send(JSON.stringify(commandData));
            console.log(`[WS SEND] Comando enviado a Web de ${targetUserId}`);
            return true;
        }

        console.log(`[WS WARN] Usuario ${targetUserId} no conectado.`);
        return false;
    }
}