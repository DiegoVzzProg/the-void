import { defineStore } from "pinia";

export const useUserStorage = defineStore("userStorage", {
  state: () => ({
    username: "" as string,
    email: "" as string,
  }),
  actions: {
    set(username: string, email: string) {
      this.username = username;
      this.email = email;
    },
  },
  persist: true,
});
