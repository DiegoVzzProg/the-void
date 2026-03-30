import { IUser } from "@/modules/users";

export interface IRegister {
    username: string;
    email: string;
    password: string;
}

export interface IAuthResponse {
    user: IUser;
    accessToken: string;
    refreshToken: string;
    hwid: string;
}

export interface ILogin {
    email: string;
    password: string;
}

export interface IRefreshToken {
    accessToken: string;
}

export interface IAuthState {
    user: IUser | null;
    isAuthenticated: boolean;
    setUser: (user: IUser | null) => void;
}