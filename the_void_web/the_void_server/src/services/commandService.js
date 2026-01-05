const prisma = require("../config/prisma");

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
      },
      create: {
        user_id: id,
        module: data.module,
        action: data.action,
        settings: data.settings,
      },
    });
  }
}

module.exports = new CommandService();
