import { ILoginParams, IRegisterParams, ITokenPayload } from "@/modules/auth/auth.interface";
import { Config } from "@/shared/config/config";
import prisma from "@/shared/database/prisma";
import { AppError } from "@/shared/utils/app-error.utils";
import { TokenManager } from "@/shared/utils/token-manager.utils";
import bcrypt from "bcrypt";

export class AuthService {
    public static async Me(user: ITokenPayload) {
        const userData = await prisma.void_users.findUnique({
            where: { id: user.id },
        });

        if (!userData) {
            throw new AppError("User not found", 404, "Not Found");
        }

        return {
            username: userData.username,
            email: userData.email,
            role: userData.role,
        };
    }

    public static async RegisterUser(params: IRegisterParams) {
        const { email, password, username } = params;

        return await prisma.$transaction(async (db) => {

            const existsUser = await db.void_users.findUnique({
                where: { email: email },
            });

            if (existsUser) {
                throw new AppError("The user or email is already registered", 409, "Conflict");
            }
            const contieneCaracteresInvisibles = /[\x00-\x1F\x7F]/.test(password);

            if (contieneCaracteresInvisibles) {
                throw new AppError("Invalid password", 400, "Bad Request");
            }

            const hashedPassword = await bcrypt.hash(password, Config.SALT_ROUNDS);

            const user = await db.void_users.create({
                data: {
                    username: username,
                    email: email,
                    password: hashedPassword,
                    banned: false,
                    hwid: null,
                    role: "USER"
                }
            });

            const tokens = TokenManager.GenerateTokens(user, "WEB_SESSION");

            return {
                user: {
                    email: user.email,
                    username: user.username,
                },
                accessToken: tokens.accessToken,
                refreshToken: tokens.refreshToken,
            };
        });
    }

    public static async LoginUser(params: ILoginParams) {
        const { email, password, ticket_id } = params;
        if (!email || !password) {
            throw new AppError("Invalid credentials", 400, "Bad Request");
        }

        let user = await prisma.void_users.findFirst({
            where: { email: email },
        });

        if (!user) {
            throw new AppError("Invalid credentials", 400, "Bad Request");
        }

        const passwordIsValid = await bcrypt.compare(password, user.password);
        if (!passwordIsValid) {
            throw new AppError("Invalid credentials", 400, "Bad Request");
        }

        if (user.banned) {
            throw new AppError("License suspended or permanently banned.", 403, "Forbidden");
        }

        const pendingTicket = await prisma.void_login_pending.findFirst({
            where: { id: ticket_id },
        });

        if (pendingTicket) {
            user = await this.ValidateHWIDLoginLoader(user, pendingTicket.temp_hwid);
        }

        const hwidDefault = user.hwid || "WEB_SESSION";
        const tokens = TokenManager.GenerateTokens(user, hwidDefault);

        return {
            accessToken: tokens.accessToken,
            refreshToken: tokens.refreshToken
        };
    }

    private static async ValidateHWIDLoginLoader(user: any, hwid: string) {
        if (hwid.startsWith("WEB_")) {
            throw new AppError("Invalid HWID", 403, "Forbidden");
        }

        if (user.hwid && user.hwid !== hwid) {
            throw new AppError("Invalid HWID", 403, "Forbidden");
        }

        return await prisma.void_users.update({
            where: { id: user.id },
            data: { hwid: hwid },
        });
    }

    public static async ValidateHWIDLoader(hwid: string) {
        const user = await prisma.void_users.findUnique({
            where: { hwid: hwid },
        });

        if (!user) {
            const login = await prisma.void_login_pending.create({
                data: {
                    temp_hwid: hwid,
                    created_at: new Date(),
                    expires_at: new Date(Date.now() + 1000 * 60 * 15),
                },
            });

            return {
                found: false,
                token: login.id,
                error: null
            }
        }

        if (user.banned || user.ban_reason) {
            throw new AppError(user.ban_reason ?? "License suspended or permanently banned.", 403, "Forbidden");
        }

        const subscription = await prisma.void_subscriptions.findFirst({
            where: {
                user_id: user.id,
                active: true,
                expires_at: {
                    gte: new Date(),
                }
            },
        });

        if (!subscription) {
            throw new AppError("No active subscription found.", 403, "Forbidden");
        }

        return {
            found: true,
            token: user.hwid,
            error: null
        }
    }

    public static async RefreshToken(headers: any) {
        const currentRefreshToken = headers["x-refresh-token"];
        const refreshToken = Array.isArray(currentRefreshToken) ? currentRefreshToken[0] : currentRefreshToken;

        if (!refreshToken) {
            throw new AppError("No refresh token provided", 401, "Unauthorized");
        }

        const decoded = TokenManager.VerifyRefreshToken(refreshToken);

        if (!decoded) {
            throw new AppError("Invalid refresh token", 401, "Unauthorized");
        }

        const user = await prisma.void_users.findUnique({
            where: { id: decoded.id },
        });

        if (!user) {
            throw new AppError("User not found", 404, "Not Found");
        }

        const tokens = TokenManager.GenerateTokens(user, decoded.hwid);

        return {
            accessToken: tokens.accessToken,
            refreshToken: tokens.refreshToken
        };
    }
}