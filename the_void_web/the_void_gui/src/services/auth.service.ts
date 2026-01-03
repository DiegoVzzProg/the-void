import type { IApiRequest, IApiResponse } from "../interfaces/api.interfaces";
import type {
  IRegister,
  IAuthResponse,
  ILogin,
} from "../interfaces/auth.interface";
import { Api } from "./api.service";

export class AuthService {
  private static apiInstance: Api;
  private readonly prefix: string = "/auth";

  private static get api(): Api {
    if (!AuthService.apiInstance) {
      AuthService.apiInstance = Api.getInstance();
    }
    return AuthService.apiInstance;
  }

  public async Register(data: IRegister): Promise<IApiResponse<IAuthResponse>> {
    const params: IApiRequest = {
      method: "POST",
      endpoint: `${this.prefix}/register`,
      body: data,
    };

    const response = await AuthService.api.Request<IAuthResponse>(params);

    return response;
  }

  public async Login(data: ILogin): Promise<IApiResponse<IAuthResponse>> {
    const params: IApiRequest = {
      method: "POST",
      endpoint: `${this.prefix}/login`,
      body: data,
    };

    const response = await AuthService.api.Request<IAuthResponse>(params);

    return response;
  }
}
