import { FastifyInstance } from "fastify";
import { CommandController } from "./command.controller";
import authenticate from "@/middlewares/auth.middleware";
import { CommandSchema } from "./command.schemas";

class Config {
    public static ProcessCommand = {
        schema: CommandSchema,
        config: {
            rateLimit: {
                max: 10,
                timeWindow: "1m",
            },
        },
    }

    public static LoadConfig = {
        onRequest: [authenticate],
        config: {
            rateLimit: {
                max: 10,
                timeWindow: "1m",
            },
        },
    }
}

export class CommandRoutes {
    private static readonly PREFIX: string = "/command";

    public static async Routes(fastify: FastifyInstance) {
        fastify.post(
            `${this.PREFIX}`,
            Config.ProcessCommand,
            CommandController.ProcessCommand
        );

        fastify.get(
            `${this.PREFIX}/load-config`,
            Config.LoadConfig,
            CommandController.LoadConfig
        );
    }
}