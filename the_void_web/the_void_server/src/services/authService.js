const prisma = require("../config/prisma"); // Importamos el Singleton
const bcrypt = require("bcrypt");
const TokenManager = require("../utils/tokenManager");

const SALT_ROUNDS = 10;

class AuthService {
  /**
   * Registra un nuevo usuario en la base de datos
   * @param {Object} params - { username, email, password }
   */
  async registerUser(params) {
    try {
      const { email, password, username } = params;

      return await prisma.$transaction(async (tx) => {
        // 1. Verificar duplicados
        const existsUser = await prisma.void_users.findUnique({
          where: { email: email },
        });

        if (existsUser) {
          throw new Error("The user or email is already registered");
        }

        // 2. Hashear password
        const hashedPassword = await bcrypt.hash(password, SALT_ROUNDS);

        // 3. Crear usuario (HWID se queda en null por defecto hasta el primer login)
        const user = await prisma.void_users.create({
          data: {
            username: username,
            email: email,
            password: hashedPassword,
            banned: false,
            hwid: null,
          },
        });

        const tokens = TokenManager.generateTokens(user, "WEB_SESSION");

        return {
          user: {
            email: user.email,
            username: user.username,
          },
          accessToken: tokens.accessToken,
          refreshToken: tokens.refreshToken,
        };
      });
    } catch (error) {
      throw error;
    }
  }

  /**
   * Login con validación y vinculación de HWID
   * @param {Object} params - { email, password, hwid }
   */
  async loginUser(params, hwid) {
    const { email, password } = params;
    if (!email || !password) {
      throw new Error("Invalid credentials");
    }

    let user = await prisma.void_users.findUnique({
      where: { email: email },
    });

    if (!user) {
      throw new Error("Invalid credentials");
    }

    if (user.banned) {
      throw new Error("License suspended or permanently banned.");
    }

    const isMatch = await bcrypt.compare(password, user.password);
    if (!isMatch) {
      throw new Error("Invalid credentials");
    }

    if (hwid && !hwid.startsWith("WEB-")) {
      if (!user.hwid) {
        user = await prisma.void_users.update({
          where: { id: user.id },
          data: { hwid: hwid },
        });
      } else {
        if (user.hwid !== hwid) {
          throw new Error("HWID Mismatch: Your license is tied to another PC.");
        }
      }
    } else {
      // Permitimos el login web sin validar HWID.
      // Opcional: Podrías restringir acciones sensibles en la web si quieres.
    }

    const tokens = TokenManager.generateTokens(
      user,
      user.hwid || "WEB_SESSION"
    );

    return {
      user: {
        email: user.email,
        username: user.username,
      },
      accessToken: tokens.accessToken,
      refreshToken: tokens.refreshToken,
    };
  }

  async refreshToken(params) {
    const valid = TokenManager.verifyRefreshToken(params.refresh_token);
    if (!valid) throw new Error("Expired token");

    const user = {
      id: valid.id,
      hwid: valid.hwid,
    };

    const tokens = TokenManager.generateTokens(
      user,
      user.hwid || "WEB_SESSION"
    );

    return {
      accessToken: tokens.accessToken,
    };
  }
}

module.exports = new AuthService();
