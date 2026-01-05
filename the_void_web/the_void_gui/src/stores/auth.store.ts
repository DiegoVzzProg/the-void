import { useCookies } from "@vueuse/integrations/useCookies";
import { defineStore } from "pinia";

const authCookies = useCookies(["__voidRefreshToken", "__voidAccessToken"]);
export const useAuthStorage = defineStore("authStorage", {
  state: () => ({
    optionActive: true as boolean,
  }),
  actions: {
    deactivateOption(value: "yes" | "no") {
      this.optionActive = value === "yes" ? false : true;
    },
    saveToken(
      name: "__voidRefreshToken" | "__voidAccessToken",
      value: string,
      maxAge: number
    ) {
      authCookies.set(name, value, {
        path: "/",
        maxAge: maxAge, //
        secure: true,
        sameSite: "strict",
      });
    },
    deleteToken(name: "__voidRefreshToken" | "__voidAccessToken") {
      authCookies.remove(name);
    },
    getCookie(name: "__voidRefreshToken" | "__voidAccessToken") {
      return authCookies.get(name);
    },
  },
  persist: true,
});
