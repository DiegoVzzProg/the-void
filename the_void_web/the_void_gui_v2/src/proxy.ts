import { NextResponse } from 'next/server';
import type { NextRequest } from 'next/server';

const routePrefix: string = 'system';
const API_URL = process.env.NEXT_PUBLIC_API_URL || "";

const protectedRoutes: string[] = [
    `/${routePrefix}/dashboard`,
];

const authRoutes: string[] = ['/auth'];

function getCookiePrefix(isProduction: boolean): string {
    return isProduction ? "__Host-" : "__Test-";
}

async function refreshAccessToken(
    refreshToken: string,
    accessToken: string | undefined,
): Promise<string | null> {
    try {
        const headers: Record<string, string> = {
            "Content-Type": "application/json",
            "x-refresh-token": refreshToken,
        };

        if (accessToken) {
            headers.Authorization = `Bearer ${accessToken}`;
        }

        const response = await fetch(`${API_URL}/auth/refresh-token`, {
            method: "GET",
            headers,
        });

        if (!response.ok) return null;

        const data = await response.json().catch(() => null);
        return data?.accessToken ?? null;
    } catch {
        return null;
    }
}

export async function proxy(request: NextRequest) {
    return NextResponse.next();
    // const currentPath = request.nextUrl.pathname;
    // const isProduction = process.env.NODE_ENV === "production";
    // const prefix = getCookiePrefix(isProduction);

    // const accessToken = request.cookies.get(`${prefix}access_token`)?.value;
    // const refreshToken = request.cookies.get(`${prefix}refresh_token`)?.value;

    // const isProtectedRoute = protectedRoutes.some(route => currentPath.startsWith(route));
    // const isAuthRoute = authRoutes.some(route => currentPath.startsWith(route));

    // if (accessToken) {
    //     if (isAuthRoute) {
    //         return NextResponse.redirect(new URL('/', request.url));
    //     }
    //     return NextResponse.next();
    // }

    // // No tiene access_token pero tiene refresh_token → intentar refrescar
    // if (!accessToken && refreshToken) {
    //     const newAccessToken = await refreshAccessToken(refreshToken, accessToken);

    //     if (newAccessToken) {
    //         const response = isAuthRoute
    //             ? NextResponse.redirect(new URL('/', request.url))
    //             : NextResponse.next();

    //         response.cookies.set(`${prefix}access_token`, newAccessToken, {
    //             expires: new Date(Date.now() + 15 * 60 * 1000),
    //             secure: isProduction,
    //             sameSite: "strict",
    //             httpOnly: true,
    //             path: "/",
    //         });

    //         return response;
    //     }

    //     // Refresh falló → limpiar cookies y redirigir si es ruta protegida
    //     const response = isProtectedRoute
    //         ? NextResponse.redirect(new URL('/auth', request.url))
    //         : NextResponse.next();

    //     response.cookies.delete(`${prefix}refresh_token`);
    //     response.cookies.delete(`${prefix}access_token`);
    //     return response;
    // }

    // // No tiene ningún token
    // if (isProtectedRoute) {
    //     return NextResponse.redirect(new URL('/auth', request.url));
    // }

    // return NextResponse.next();
}

export const config = {
    matcher: [
        '/((?!api|_next/static|_next/image|favicon.ico|.*\\.png|.*\\.jpg).*)',
    ],
};