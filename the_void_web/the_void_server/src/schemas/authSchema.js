const login = {
  body: {
    type: "object",
    required: ["username", "password", "hwid"], // HWID obligatorio para login
    properties: {
      username: { type: "string" },
      password: { type: "string" },
      hwid: { type: "string", minLength: 5 }, // Un HWID real es largo
    },
  },
};

const register = {
  body: {
    type: "object",
    required: ["username", "password", "email"],
    properties: {
      username: { type: "string" },
      password: { type: "string" },
      email: { type: "string" },
    },
  },
};

const refresh_token = {
  body: {
    type: "object",
    required: ["refresh_token"],
    properties: {
      refresh_token: { type: "string" },
    },
  },
};
