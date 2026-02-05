export const login = {
  body: {
    type: "object",
    required: ["email", "password"],
    properties: {
      email: { type: "string" },
      password: { type: "string" },
    },
  },
};

export const register = {
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

export const refresh_token = {
  body: {
    type: "object",
    required: ["refresh_token"],
    properties: {
      refresh_token: { type: "string" },
    },
  },
};
