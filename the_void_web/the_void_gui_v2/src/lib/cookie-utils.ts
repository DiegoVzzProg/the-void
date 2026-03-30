const REFRESH_TOKEN_EXPIRY = 7 * 24 * 60 * 60 * 1000; // 7 days
const ACCESS_TOKEN_EXPIRY = 15 * 60 * 1000; // 15 minutes

export function getCookiePrefix(): string {
    const isProduction = process.env.NODE_ENV === "production";
    return isProduction ? "__Host-" : "__Test-";
}

export function getCookieOptions(expiresMs: number) {
    const isProduction = process.env.NODE_ENV === "production";
    return {
        expires: new Date(Date.now() + expiresMs),
        secure: isProduction,
        sameSite: "strict" as const,
        httpOnly: true,
        path: "/",
    };
}

export { REFRESH_TOKEN_EXPIRY, ACCESS_TOKEN_EXPIRY };
