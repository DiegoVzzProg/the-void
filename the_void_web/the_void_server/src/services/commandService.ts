import prisma from "../config/prisma";
import TokenManager from "../utils/tokenManager";

class CommandService {
  async saveConfig(data: any, id: string) {
    return await prisma.void_module_config.upsert({
      where: {
        user_id_module: {
          user_id: id,
          module: data.module as any,
        },
      },
      update: {
        action: data.action,
        settings: data.settings,
        conditions: data.conditions,
      },
      create: {
        user_id: id,
        module: data.module as any,
        action: data.action,
        settings: data.settings,
        conditions: data.conditions,
      },
    });
  }

  async loadConfig(authHeader: string) {
    const token = authHeader.replace("Bearer ", "");
    const decoded = TokenManager.verifyAccessToken(token);
    
    if (!decoded || typeof decoded === 'string') {
        throw new Error("Invalid token");
    }

    const userId = (decoded as any).id; // Cast since verifyAccessToken returns general JwtPayload

    const data = await prisma.void_users.findUnique({
      where: { id: userId },
    });

    if (!data) throw new Error("Unregistered user");

    const userSettings = await prisma.void_module_config.findMany({
      where: { user_id: userId },
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

export default new CommandService();
