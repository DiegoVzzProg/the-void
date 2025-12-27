import type { IApiRequest, IApiResponse } from "../interfaces/api.interfaces";
import { Api } from "./api.service";

export class CommandModuleService {
  private static apiInstance: Api;

  private static get api(): Api {
    if (!CommandModuleService.apiInstance) {
      CommandModuleService.apiInstance = Api.getInstance();
    }
    return CommandModuleService.apiInstance;
  }

  public async Command(module: string, value: any): Promise<IApiResponse<any>> {
    const params: IApiRequest = {
      method: "POST",
      endpoint: `command`,
      body: {
        module: module,
        value: value,
      },
    };
    const response = await CommandModuleService.api.Request<any>(params);
    return response;
  }
}
