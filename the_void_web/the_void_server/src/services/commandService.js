const prisma = require("../config/prisma");
const TokenManager = require("../utils/tokenManager");

class CommandService {
  async saveConfig(data, id) {
    return await prisma.void_module_config.upsert({
      where: {
        user_id_module: {
          user_id: id,
          module: data.module,
        },
      },
      update: {
        action: data.action,
        settings: data.settings,
        conditions: data.conditions,
      },
      create: {
        user_id: id,
        module: data.module,
        action: data.action,
        settings: data.settings,
        conditions: data.conditions,
      },
    });
  }

  async loadConfig(authHeader) {
    const token = authHeader.replace("Bearer ", "");
    const decoded = TokenManager.verifyAccessToken(token);
    const userId = decoded.id;

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

module.exports = new CommandService();
