const jwt = require("jsonwebtoken");
// Secretos desde .env (¡NUNCA LOS SUBAS A GITHUB!)
const ACCESS_SECRET = process.env.JWT_ACCESS_SECRET || "VOID_ACCESS_KEY_@123";
const REFRESH_SECRET =
  process.env.JWT_REFRESH_SECRET || "VOID_REFRESH_KEY_#999";

const TokenManager = {
  /**
   * Genera el par de tokens (Access + Refresh)
   * @param {Object} user - Datos del usuario (id, role, username)
   * @param {String} hwid - La huella digital de la máquina
   */
  generateTokens: (user, hwid) => {
    const payload = {
      id: user.id,
      hwid: hwid,
    };

    // 1. Access Token: Vida corta (15 minutos)
    // Si se lo roban, solo sirve por un rato.
    const accessToken = jwt.sign(payload, ACCESS_SECRET, { expiresIn: "15m" });

    // 2. Refresh Token: Vida larga (7 días)
    // Sirve para pedir un nuevo access token sin loguearse de nuevo.
    const refreshToken = jwt.sign(payload, REFRESH_SECRET, { expiresIn: "7d" });

    return { accessToken, refreshToken };
  },

  /**
   * Verifica si un Access Token es válido
   */
  verifyAccessToken: (token) => {
    try {
      return jwt.verify(token, ACCESS_SECRET);
    } catch (error) {
      return null; // Token expirado o manipulado
    }
  },

  /**
   * Verifica si un Refresh Token es válido
   */
  verifyRefreshToken: (token) => {
    try {
      return jwt.verify(token, REFRESH_SECRET);
    } catch (error) {
      return null;
    }
  },
};

module.exports = TokenManager;
