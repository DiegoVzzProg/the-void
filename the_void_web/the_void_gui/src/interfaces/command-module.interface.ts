export interface ICommand {
  module: string;
  enabled: boolean;
  value: string | number;
  conditions?: any[] | null;
}
