export interface ICommand {
  module: string;
  action: "enable" | "disable";
  settings: {
    cps?: number;

    range?: number;
    hitchance?: number;

    fov?: number;
    smoothness?: number;
    strafe_assist_value?: number;

    horizontal?: number;
    vertical?: number;
    chance?: number;
  };
  conditions?: string[];
}

export interface ICommandResponse {
  status: string;
}

export interface ILoadConfigResponse {
  module: string;
  action: "enable" | "disable";
  settings: Record<string, any>;
  conditions: string[];
}
