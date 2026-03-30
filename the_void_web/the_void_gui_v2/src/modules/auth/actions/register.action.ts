"use server";

import { AuthService, IRegister } from "@/modules/auth";
import { CookieService } from "@/api/cookie.service";

export async function RegisterServer(from: IRegister) {
    const res = await AuthService.RegisterUser(from);
    const data = res.data;

    if (data) {
        await CookieService.SetAuthCookies(data.accessToken, data.refreshToken);
    }

    return res;
}