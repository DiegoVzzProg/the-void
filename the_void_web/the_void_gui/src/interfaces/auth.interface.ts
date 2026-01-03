import type { IUser } from "./user.interface";

export interface IAuthResponse {
  user: IUser;
  accessToken: string;
  refreshToken: string;
}

export interface IRegister {
  username: string;
  email: string;
  password: string;
}

export interface ILogin {
  email: string;
  password: string;
}
