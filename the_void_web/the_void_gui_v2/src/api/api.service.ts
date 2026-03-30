import { IApiErrorDetails, IApiRequest, IApiResponse } from "./api.interface";
import { CookieService } from "./cookie.service";

const BASE_URL = process.env.NEXT_PUBLIC_API_URL || "";
const TIMEOUT_MS = 30_000;
const REFRESH_ENDPOINT = "/auth/refresh-token";


export class ApiService {
    private static ok<T>(data: T): IApiResponse<T> {
        return { success: true, data, message: "Request successful" }
    }
    private static fail<T>(message: string, errorType?: string, errorDetails?: IApiErrorDetails): IApiResponse<T> {
        return { success: false, data: null, message, errorType, errorDetails }
    }
    private static async SafeJson(response: Response): Promise<any> {
        return response.json().catch(() => null)
    }
    private static async ParseResponse<T>(response: Response): Promise<IApiResponse<T>> {
        const data = await this.SafeJson(response)

        if (response.ok) return this.ok(data)

        return this.fail(
            data?.message || response.statusText || "Unknown error occurred",
            data?.error || "Error",
            data?.details,
        )
    }

    private static async ExecuteFetch(
        method: string,
        endpoint: string,
        body?: Record<string, any>,
        headers: Record<string, string> = {},
        authHeaders: Record<string, string> = {},
    ): Promise<Response> {
        const controller = new AbortController()
        const timeoutId = setTimeout(() => controller.abort(), TIMEOUT_MS)

        try {
            return await fetch(`${BASE_URL}${endpoint}`, {
                method,
                headers: { "Content-Type": "application/json", ...authHeaders, ...headers },
                body: body ? JSON.stringify(body) : undefined,
                signal: controller.signal,
                cache: "no-store",
            })
        } finally {
            clearTimeout(timeoutId)
        }
    }

    private static async TryRefreshToken(authHeaders: Record<string, string>): Promise<string | null> {
        try {
            const response = await this.ExecuteFetch("GET", REFRESH_ENDPOINT, undefined, {}, authHeaders)
            if (!response.ok) return null

            const data = await this.SafeJson(response)
            return data?.accessToken ?? null
        } catch {
            return null
        }
    }

    public static async ApiRequest<T>({
        method,
        endpoint,
        body,
        headers = {},
    }: IApiRequest): Promise<IApiResponse<T>> {
        const authHeaders = await CookieService.GetAuthHeaders()
        
        try {
            const response = await this.ExecuteFetch(method, endpoint, body, headers, authHeaders)

            if (response.status === 401 && endpoint !== REFRESH_ENDPOINT) {
                const newToken = await this.TryRefreshToken(authHeaders)

                if (!newToken) {
                    const data = await this.SafeJson(response)
                    return this.fail<T>(data?.message || "Unauthorized", "Unauthorized")
                }

                const retryAuth = { ...authHeaders, Authorization: `Bearer ${newToken}` }
                return this.ParseResponse<T>(await this.ExecuteFetch(method, endpoint, body, headers, retryAuth))
            }
            
            return this.ParseResponse<T>(response)
        } catch (error: any) {
            if (error.name === "AbortError") {
                return this.fail<T>("Request timed out", "Timeout")
            }
            return this.fail<T>(error.message || "Network error or lost connection", "Network")
        }
    }
}