import { ApiService } from "@/api/api.service";
import { IAuthResponse, ILogin, IRefreshToken, IRegister } from "@/modules/auth";
import { IUser } from "@/modules/users";

export class AuthService {
    private static readonly PREFIX = "/auth";
    public static async RegisterUser(data: IRegister) {
        return await ApiService.ApiRequest<IAuthResponse>({
            method: "POST",
            endpoint: `${this.PREFIX}/register`,
            body: data,
        });
    }

    public static async LoginUser(data: ILogin) {
        return await ApiService.ApiRequest<IAuthResponse>({
            method: "POST",
            endpoint: `${this.PREFIX}/login`,
            body: data,
        });
    }

    public static async Me() {
        const ls = await ApiService.ApiRequest<IUser>({
            method: "GET",
            endpoint: `${this.PREFIX}/me`,
        });
        return ls;
    }

    public static async RefreshToken() {
        return await ApiService.ApiRequest<IRefreshToken>({
            method: "GET",
            endpoint: `${this.PREFIX}/refresh-token`,
        });
    }
}