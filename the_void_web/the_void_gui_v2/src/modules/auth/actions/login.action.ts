"use server";

import { AuthService, ILogin } from "@/modules/auth";
import { CookieService } from "@/api/cookie.service";

export async function LoginServer(from: ILogin) {
    const res = await AuthService.LoginUser(from);
    const data = res.data;

    if (data) {
        await CookieService.SetAuthCookies(data.accessToken, data.refreshToken);
    }

    return res;
}