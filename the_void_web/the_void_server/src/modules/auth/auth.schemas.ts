import { z } from "zod";

export const AuthSchema = {
    Login: {
        body: z.object({
            email: z.string().email("Invalid email"),
            password: z.string().min(6, "Password is too short"),
        }),
    },
    Register: {
        body: z.object({
            username: z.string().min(3),
            email: z.string().email(),
            password: z.string().min(6),
        }),
    },
    Loader: {
        params: z.object({
            hwid: z.string().uuid("Invalid HWID"),
        }),
    }
};