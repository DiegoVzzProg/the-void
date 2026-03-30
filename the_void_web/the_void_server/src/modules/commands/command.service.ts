import prisma from "@/shared/database/prisma";
import { TokenManager } from "@/shared/utils/token-manager.utils";
import { ITokenPayload } from "../auth";
import { AppError } from "@/shared/utils/app-error.utils";
import { WsManager } from "@/shared/plugins/ws-manager.plugin";

export class CommandService {
    public static async ProcessCommand(user: ITokenPayload | null, data: any) {
        if (!user) throw new AppError("Unregistered user", 401, "Unauthorized");

        if (!user.hwid) throw new AppError("Missing hwid", 400, "Missing hwid");

        const send = await WsManager.SendCommandToClient(user.hwid, data);

        if (send) {
            console.log("[API] Comando enviado al Socket exitosamente.");
        } else {
            console.warn("[API] Se guardó config pero el cliente C++ no parece conectado.");
        }

        return { success: true };
    }
    public static async SaveConfig(data: any, id: string) {
        let profile = await prisma.void_profiles.findFirst({
            where: { user_id: id, is_default: true }
        });

        if (!profile) {
            profile = await prisma.void_profiles.create({
                data: {
                    user_id: id,
                    name: "Default Profile",
                    is_default: true,
                }
            });
        }

        return await prisma.void_module_config.upsert({
            where: {
                profile_id_module: {
                    profile_id: profile.id,
                    module: data.module as any,
                },
            },
            update: {
                action: data.action,
                settings: data.settings ?? {},
                conditions: data.conditions ?? [],
            },
            create: {
                profile_id: profile.id,
                module: data.module as any,
                action: data.action,
                settings: data.settings ?? {},
                conditions: data.conditions ?? [],
            },
        });
    }

    public static async LoadConfig(authHeader: string | undefined) {
        if (!authHeader) throw new AppError("Missing token", 400, "Missing token");

        const token = authHeader.replace("Bearer ", "");
        const decoded = TokenManager.VerifyAccessToken(token);

        if (!decoded) {
            throw new AppError("Invalid token", 401, "Unauthorized");
        }

        const userId = decoded.id;

        const data = await prisma.void_users.findUnique({
            where: { id: userId },
        });

        if (!data) throw new AppError("Unregistered user", 404, "Not Found");

        const profile = await prisma.void_profiles.findFirst({
            where: { user_id: userId, is_default: true }
        });

        if (!profile) return [];

        const userSettings = await prisma.void_module_config.findMany({
            where: { profile_id: profile.id },
            select: {
                module: true,
                action: true,
                settings: true,
                conditions: true,
            },
        });

        return userSettings;
    }
}