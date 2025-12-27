export interface IApiResponse<T = any> {
  data: T | null;
  message?: string;
  error?: Record<any, any>;
}

export interface IApiRequest {
  method: "GET" | "POST" | "PUT" | "DELETE";
  endpoint: string;
  body?: Record<string, any>;
  headers?: Record<string, string>;
}
