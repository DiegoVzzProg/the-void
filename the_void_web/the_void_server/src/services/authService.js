const prisma = require("../config/prisma"); // Importamos el Singleton
const bcrypt = require("bcrypt");
const jwt = require("jsonwebtoken");

const SALT_ROUNDS = 10;
const JWT_SECRET = process.env.JWT_SECRET || "SUPER_SECRET_KEY_THE_VOID";

class AuthService {
  /**
   * Registra un nuevo usuario en la base de datos
   * @param {Object} params - { username, email, password }
   */
  async registerUser(params) {
    try {
      const { email, password, username } = params;

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
      const newUser = await prisma.void_users.create({
        data: {
          username: username,
          email: email,
          password: hashedPassword,
          banned: false,
          hwid: null, // Explícito para claridad
        },
      });

      return { success: true, message: "Successfully registered user" };
    } catch (error) {
      throw error;
    }
  }

  /**
   * Login con validación y vinculación de HWID
   * @param {Object} params - { email, password, hwid }
   */
  async loginUser(params) {
    const { email, password, hwid } = params; // IMPORTANTE: Aquí extraemos el hwid que viene del cliente C++

    // 1. Buscar usuario
    const user = await prisma.void_users.findUnique({
      where: { email: email },
    });

    if (!user) {
      throw new Error("Invalid credentials");
    }

    // 2. Verificar Baneo
    if (user.banned) {
      throw new Error("License suspended or permanently banned.");
    }

    // 3. Verificar Contraseña
    const isMatch = await bcrypt.compare(password, user.password);
    if (!isMatch) {
      throw new Error("Invalid credentials");
    }

    // 4. --- LÓGICA DE HWID (GHOST CLIENT) ---
    if (!user.hwid) {
      await prisma.void_users.update({
        where: { id: user.id },
        data: { hwid: hwid },
      });
    } else {
      if (user.hwid !== hwid) {
        throw new Error(
          "HWID Mismatch: Your license is tied to another PC. Contact support to reset."
        );
      }
    }

    const token = jwt.sign(
      { id: user.id, role: user.role, hwid: hwid }, // Guardamos el HWID en el token también
      JWT_SECRET,
      { expiresIn: "24h" }
    );

    return {
      token,
      user: {
        id: user.id,
        username: user.username,
        role: user.role,
      },
    };
  }
}

module.exports = new AuthService();
