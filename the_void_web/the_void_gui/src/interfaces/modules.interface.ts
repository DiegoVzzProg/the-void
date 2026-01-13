export type Action = "enable" | "disable";

export interface IAutoClicker {
  action: Action;
  cps: number;
  conditions: string[];
}

export interface IReach {
  action: Action;
  range: number;
  hitchance: number;
  conditions: string[];
}

export interface IFirstHit {
  action: Action;
  range: number;
}

export interface IAimAssist {
  action: Action;
  range: number;
  fov: number;
  smoothness: number;
  strafe_assist_value: number;
  conditions: string[];
}
