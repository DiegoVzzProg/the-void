const authService = require("../services/authService");

const authController = {
  register: async (req, reply) => {
    const result = await authService.registerUser(req.body);
    return reply.code(201).send(result);
  },

  login: async (req, reply) => {
    const currentHwid = req.headers["x-hwid"];
    const result = await authService.loginUser(req.body, currentHwid);
    return reply.code(200).send(result);
  },

  refreshToken: async (req, reply) => {
    const result = await authService.refreshToken(req.body);
    return reply.code(200).send(result);
  },
};

module.exports = authController;
