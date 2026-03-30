"use server";

import { AuthService } from "@/modules/auth";

export async function fetchSession() {
    const response = await AuthService.Me();
    if (response.success && response.data) {
        return response.data;
    } else {
        return null;
    }
}