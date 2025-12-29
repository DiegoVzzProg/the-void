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

  public async Command(
    commands: any
  ): Promise<IApiResponse<{ response: string }>> {
    const params: IApiRequest = {
      method: "POST",
      endpoint: `command`,
      body: commands,
    };

    const response = await CommandModuleService.api.Request<{
      response: string;
    }>(params);

    return response;
  }
}
