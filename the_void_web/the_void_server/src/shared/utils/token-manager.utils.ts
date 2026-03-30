import { ITokenPayload } from '@/modules/auth/auth.interface';
import jwt from 'jsonwebtoken';
import { AppError } from './app-error.utils';

const ACCESS_SECRET = process.env.JWT_ACCESS_SECRET as string;
const REFRESH_SECRET = process.env.JWT_REFRESH_SECRET as string;

if (!ACCESS_SECRET || !REFRESH_SECRET) {
    throw new AppError("FATAL: Missing JWT secrets in environment variables.", 500, "Internal Server Error");
}

interface UserData {
    id: string;
    role: string;
}

export class TokenManager {
    public static GenerateTokens(user: UserData, hwid: string) {
        const payload: ITokenPayload = {
            id: user.id,
            role: user.role,
            hwid: hwid,
        };

        const accessToken = jwt.sign(payload, ACCESS_SECRET, { expiresIn: "1h" });
        const refreshToken = jwt.sign(payload, REFRESH_SECRET, { expiresIn: "7d" });

        return { accessToken, refreshToken };
    }

    public static VerifyAccessToken(token: string): ITokenPayload | null {
        try {
            return jwt.verify(token, ACCESS_SECRET) as ITokenPayload;
        } catch (error) {
            return null;
        }
    }

    public static VerifyRefreshToken(token: string): ITokenPayload | null {
        try {
            return jwt.verify(token, REFRESH_SECRET) as ITokenPayload;
        } catch (error) {
            return null;
        }
    }
}