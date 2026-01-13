import axios, {
  AxiosError,
  type AxiosInstance,
  type AxiosResponse,
} from "axios";
import type { IApiRequest, IApiResponse } from "../interfaces/api.interfaces";
import { useAuthStorage } from "../stores/auth.store";
import { AuthService } from "./auth.service";
export class Api {
  private static instance: Api | null = null;
  private static axios: AxiosInstance;

  private constructor() {
    this.InitializeInstanceAxios();
    this.InitializeInterceptors();
  }

  public static getInstance(): Api {
    if (!Api.instance) {
      Api.instance = new Api();
    }
    return Api.instance;
  }

  private InitializeInstanceAxios(): void {
    const apiUrl: string =
      import.meta.env.VITE_API_URL_LOCAL ?? "http://localhost:5010/api/";

    if (!apiUrl) {
      throw new Error("API url is not defined");
    }

    Api.axios = axios.create({
      baseURL: apiUrl,
      timeout: 300000,
    });
  }

  private InitializeInterceptors(): void {
    Api.axios.interceptors.request.use(
      async (config) => {
        const token: string = useAuthStorage().getCookie("__voidAccessToken");
        if (token) {
          config.headers["Authorization"] = `Bearer ${token}`;
          config.headers["x-hwid"] = `WEB_SESSION`;
        }

        return config;
      },
      (error) => {
        return Promise.reject(error);
      }
    );
    Api.axios.interceptors.response.use(
      (response: AxiosResponse) => response,
      async (error: AxiosError) => {
        const originalRequest = error.config; // 1. Guardamos la configuración de la petición fallida
        const { response } = error;

        // Si no hay respuesta o error de red, rechazamos
        if (!response) return Promise.reject(error);

        // 2. Verificamos si es 401 y si NO es un reintento (para evitar bucles infinitos)
        if (
          response.status === 401 &&
          originalRequest &&
          !(originalRequest as any)._retry
        ) {
          // Marcamos esta petición como "ya reintentada"
          (originalRequest as any)._retry = true;

          try {
            // Intentamos refrescar el token
            const tokens = await new AuthService().RefreshToken();
            const data = tokens.data;

            if (!data || !data.accessToken) {
              // Si el refresh falla, es buena práctica desloguear al usuario aquí
              // useAuthStorage().clear();
              // window.location.href = '/login';
              return Promise.reject(error);
            }

            // Guardamos el nuevo token
            useAuthStorage().saveToken(
              "__voidAccessToken",
              data.accessToken,
              60 * 15
            );

            // 3. ACTUALIZAMOS EL HEADER de la petición original con el nuevo token
            originalRequest.headers[
              "Authorization"
            ] = `Bearer ${data.accessToken}`;

            // 4. EJECUTAMOS DE NUEVO la petición original
            // Esto devuelve una nueva promesa que resuelve la petición original
            return Api.axios(originalRequest);
          } catch (refreshError) {
            // Si el refresh falla (ej: refresh token expirado), rechazamos el error original o redirigimos a login
            return Promise.reject(refreshError);
          }
        }

        return Promise.reject(error);
      }
    );
  }

  private handleOffline(): IApiResponse {
    console.warn("offline");
    return {
      data: {
        estado: "error-red",
      },
    };
  }

  public async Request<T>(https: IApiRequest): Promise<IApiResponse<T>> {
    try {
      if (typeof navigator !== "undefined" && !navigator.onLine) {
        return this.handleOffline();
      }
      const header: Record<string, string> = https.headers ?? {
        "Content-Type": "application/json",
      };

      let axiosResponse: IApiResponse;
      switch (https.method) {
        case "GET":
          axiosResponse = await Api.axios.get(https.endpoint, header);
          break;
        case "POST":
          axiosResponse = await Api.axios.post(
            https.endpoint,
            https.body,
            header
          );
          break;
        case "PUT":
          axiosResponse = await Api.axios.put(
            https.endpoint,
            https.body,
            header
          );
          break;
        case "DELETE":
          axiosResponse = await Api.axios.delete(https.endpoint, header);
          break;
      }

      return axiosResponse;
    } catch (error: any) {
      if (axios.isAxiosError(error)) {
        const axiosError = error as AxiosError;
        if (
          !axiosError.response &&
          (axiosError.message === "Network Error" ||
            axiosError.code === "ECONNABORTED")
        ) {
          return this.handleOffline();
        }

        if (axiosError.response && axiosError.status === 422) {
          const errorsValidations: any = axiosError.response.data;
          return { data: null, error: errorsValidations.errors };
        }
      }

      const errorMessage = error.response.data.message || "Unknown error";
      console.error(errorMessage);
      //Functions.Notification({
      //  message: errorMessage,
      //  type: "error",
      //});
      return { data: null };
    }
  }
}
