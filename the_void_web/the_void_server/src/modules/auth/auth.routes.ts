import { FastifyInstance } from "fastify";
import { AuthController } from "./auth.controller";
import { AuthSchema } from "./auth.schemas";
import authenticate from "@/middlewares/auth.middleware";

class Config {
    public static Login = {
        schema: AuthSchema.Login,
        config: {
            rateLimit: {
                max: 5,
                timeWindow: "1 hour",
            },
        },
    }

    public static Register = {
        schema: AuthSchema.Register,
        config: {
            rateLimit: {
                max: 5,
                timeWindow: "1 hour",
            },
        },
    }

    public static Loader = {
        config: {
            rateLimit: {
                max: 10,
                timeWindow: "1 hour",
            },
        },
    }

    public static Me = {
        onRequest: [authenticate],
    }
}

export class AuthRoutes {
    private static readonly PREFIX: string = "/auth";

    public static async Routes(fastify: FastifyInstance) {
        fastify.post(
            `${this.PREFIX}/register`,
            Config.Register,
            AuthController.Register
        );

        fastify.post(
            `${this.PREFIX}/login`,
            Config.Login,
            AuthController.Login
        );

        fastify.get(
            `${this.PREFIX}/refresh-token`,
            AuthController.RefreshToken
        );

        fastify.get(
            `${this.PREFIX}/loader/:hwid`,
            Config.Loader,
            AuthController.Loader
        );

        fastify.get(
            `${this.PREFIX}/me`,
            Config.Me,
            AuthController.Me
        );
    }
}