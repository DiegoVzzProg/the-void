export interface IApiErrorDetails {
    [key: string]: any;
}

export interface IApiResponse<T = any> {
    success: boolean;
    data: T | null;
    message: string;
    errorType?: string;
    errorDetails?: IApiErrorDetails;
}

export interface IApiRequest {
    method: "GET" | "POST" | "PUT" | "DELETE";
    endpoint: string;
    body?: Record<string, any>;
    headers?: Record<string, string>;
}