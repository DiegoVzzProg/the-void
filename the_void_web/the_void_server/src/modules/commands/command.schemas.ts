import { z } from "zod";

export const CommandSchema = {
    body: z.object({
        module: z.string({ error: "Module is required and must be a string" }),

        action: z.enum(["enable", "disable"], {
            error: "Action is required and must be either 'enable' or 'disable'",
        }),

        settings: z.object({
            // AutoClicker properties
            cps: z.number({ error: "CPS must be a number" })
                .min(1, "CPS must be at least 1")
                .max(100, "CPS cannot exceed 100")
                .optional(),

            // Reach / AimAssist properties
            range: z.number({ error: "Range must be a number" })
                .min(0.0, "Range must be at least 0.0")
                .max(8.0, "Range cannot exceed 8.0")
                .optional(),

            hitchance: z.number({ error: "Hitchance must be a number" })
                .min(0, "Hitchance must be at least 0")
                .max(100, "Hitchance cannot exceed 100")
                .optional(),

            fov: z.number({ error: "FOV must be a number" })
                .min(0, "FOV must be at least 0")
                .max(360, "FOV cannot exceed 360")
                .optional(),

            smoothness: z.number({ error: "Smoothness must be a number" })
                .min(0, "Smoothness must be at least 0")
                .max(100, "Smoothness cannot exceed 100")
                .optional(),

            strafe_assist_value: z.number({ error: "Strafe assist value must be a number" })
                .min(0.0, "Strafe assist value must be at least 0.0")
                .max(5.0, "Strafe assist value cannot exceed 5.0")
                .optional(),

            // Velocity properties
            horizontal: z.number({ error: "Horizontal must be a number" })
                .min(0, "Horizontal must be at least 0")
                .max(100, "Horizontal cannot exceed 100")
                .optional(),

            vertical: z.number({ error: "Vertical must be a number" })
                .min(0, "Vertical must be at least 0")
                .max(100, "Vertical cannot exceed 100")
                .optional(),

            chance: z.number({ error: "Chance must be a number" })
                .min(0, "Chance must be at least 0")
                .max(100, "Chance cannot exceed 100")
                .optional(),
        }).optional(),

        // Extra conditions
        conditions: z.array(
            z.string({ error: "Each condition must be a string" })
        ).optional(),
    }),

    // Tipando también la respuesta
    response: {
        200: z.object({
            status: z.string(),
        }),
    },
};