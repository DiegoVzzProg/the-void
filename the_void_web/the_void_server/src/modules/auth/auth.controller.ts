
import { FastifyReply, FastifyRequest } from "fastify";
import { ILoginParams, IRegisterParams, ITokenPayload } from "./auth.interface";
import { AuthService } from "./auth.service";

export class AuthController {
    public static async Register(req: FastifyRequest, reply: FastifyReply) {
        const result = await AuthService.RegisterUser(req.body as IRegisterParams);
        return reply.code(201).send(result);
    }

    public static async Login(req: FastifyRequest, reply: FastifyReply) {
        const result = await AuthService.LoginUser(req.body as ILoginParams);
        return reply.code(200).send(result);
    }

    public static async RefreshToken(req: FastifyRequest, reply: FastifyReply) {
        const result = await AuthService.RefreshToken(req.headers);
        return reply.code(200).send(result);
    }

    public static async Loader(req: FastifyRequest, reply: FastifyReply) {
        const params = req.params as { hwid: string };
        const result = await AuthService.ValidateHWIDLoader(params.hwid);
        return reply.code(200).send(result);
    }

    public static async Me(req: FastifyRequest, reply: FastifyReply) {
        console.log(`Entro: ${JSON.stringify(req.user)}`);
        const result = await AuthService.Me(req.user as ITokenPayload);
        return reply.code(200).send(result);
    }
}