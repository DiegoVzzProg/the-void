import type { IApiRequest, IApiResponse } from "../interfaces/api.interfaces";
import type {
  ICommand,
  ICommandResponse,
} from "../interfaces/command-module.interface";
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
    commands: ICommand
  ): Promise<IApiResponse<ICommandResponse>> {
    const params: IApiRequest = {
      method: "POST",
      endpoint: "command",
      body: commands,
    };

    const response = await CommandModuleService.api.Request<ICommandResponse>(
      params
    );

    return response;
  }

  public async CommandSaveConfig(
    config: ICommand
  ): Promise<IApiResponse<ICommandResponse>> {
    const params: IApiRequest = {
      method: "POST",
      endpoint: "command/save-config",
      body: config,
    };

    const response = await CommandModuleService.api.Request<ICommandResponse>(
      params
    );

    return response;
  }
}
