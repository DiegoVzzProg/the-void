import jwt from 'jsonwebtoken';

// Secretos desde .env (¡NUNCA LOS SUBAS A GITHUB!)
const ACCESS_SECRET = process.env.JWT_ACCESS_SECRET as string;
const REFRESH_SECRET = process.env.JWT_REFRESH_SECRET as string;

if (!ACCESS_SECRET || !REFRESH_SECRET) {
  throw new Error("FATAL: Missing JWT secrets in environment variables.");
}

interface UserData {
  id: number | string;
  [key: string]: any;
}

interface TokenPayload {
  id: number | string;
  hwid: string;
}

const TokenManager = {
  /**
   * Genera el par de tokens (Access + Refresh)
   * @param {UserData} user - Datos del usuario (id, role, username)
   * @param {String} hwid - La huella digital de la máquina
   */
  generateTokens: (user: UserData, hwid: string) => {
    const payload: TokenPayload = {
      id: user.id,
      hwid: hwid,
    };

    // 1. Access Token: Vida corta (15 minutos)
    // Si se lo roban, solo sirve por un rato.
    const accessToken = jwt.sign(payload, ACCESS_SECRET, { expiresIn: "1h" });

    // 2. Refresh Token: Vida larga (7 días)
    // Sirve para pedir un nuevo access token sin loguearse de nuevo.
    const refreshToken = jwt.sign(payload, REFRESH_SECRET, { expiresIn: "7d" });

    return { accessToken, refreshToken };
  },

  /**
   * Verifica si un Access Token es válido
   */
  verifyAccessToken: (token: string) => {
    try {
      return jwt.verify(token, ACCESS_SECRET);
    } catch (error) {
      return null; // Token expirado o manipulado
    }
  },

  /**
   * Verifica si un Refresh Token es válido
   */
  verifyRefreshToken: (token: string) => {
    try {
      return jwt.verify(token, REFRESH_SECRET);
    } catch (error) {
      return null;
    }
  },
};

export default TokenManager;
