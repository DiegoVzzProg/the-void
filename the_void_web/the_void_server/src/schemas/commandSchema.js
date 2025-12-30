const commandSchema = {
  body: {
    type: "object",
    required: ["module", "action"],
    properties: {
      module: {
        type: "string",
      },

      action: { type: "string", enum: ["enable", "disable"] },

      settings: {
        type: "object",
        properties: {
          // Propiedades de AutoClicker
          cps: { type: "number", minimum: 1, maximum: 20 },

          // Propiedades de Reach / AimAssist
          range: { type: "number", minimum: 3.0, maximum: 8.0 },
          hitchance: { type: "number", minimum: 0, maximum: 100 },
          fov: { type: "number", minimum: 0, maximum: 360 },
          smoothness: { type: "number", minimum: 0, maximum: 100 },

          // Propiedades de Velocity
          horizontal: { type: "number", minimum: 0, maximum: 100 },
          vertical: { type: "number", minimum: 0, maximum: 100 },
          chance: { type: "number", minimum: 0, maximum: 100 },
        },
      },

      // 4. Condiciones extras (Array de strings)
      conditions: {
        type: "array",
        items: { type: "string" },
      },
    },
  },
  response: {
    200: {
      type: "object",
      properties: {
        status: { type: "string" },
      },
    },
  },
};

module.exports = commandSchema;
