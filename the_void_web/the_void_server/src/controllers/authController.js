const authService = require("../services/authService");

const authController = {
  register: async (req, reply) => {
    try {
      const result = await authService.registerUser(req.body);
      return reply.code(201).send(result);
    } catch (error) {
      return reply.code(400).send({
        error: "Registration Failed",
        message: error.message,
      });
    }
  },

  login: async (req, reply) => {
    try {
      const currentHwid = req.headers["x-hwid"];

      const result = await authService.loginUser(req.body, currentHwid);
      return reply.code(200).send(result);
    } catch (error) {
      return reply.code(401).send({
        error: "Login Failed",
        message: error.message,
      });
    }
  },

  refreshToken: async (req, reply) => {
    try {
      const result = await authService.refreshToken(req.body);
      return reply.code(200).send(result);
    } catch (error) {
      return reply.code(401).send({
        error: "Refresh Failed",
        message: error.message,
      });
    }
  },
};

module.exports = authController;
