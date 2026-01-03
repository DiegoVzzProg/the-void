import { useCookies } from "@vueuse/integrations/useCookies";

export const authCookies = () => {
  const cookies = useCookies(["__void_token_refresh"]);

  const saveTokenRefresh = (token: string) => {
    cookies.set("__void_token_refresh", token, {
      path: "/",
      maxAge: 60 * 60 * 24 * 7,
      secure: true,
      sameSite: "strict",
    });
  };

  const getTokenRefresh = () => {
    return cookies.get("__void_token_refresh");
  };

  const deleteTokenRefresh = () => {
    cookies.remove("__void_token_refresh");
  };

  return { saveTokenRefresh, getTokenRefresh, deleteTokenRefresh };
};
