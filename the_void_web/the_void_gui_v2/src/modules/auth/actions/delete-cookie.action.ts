"use server";

import { CookieService } from "@/api/cookie.service";

export async function DeleteCookieServer() {
    await CookieService.DeleteAuthCookies();
}