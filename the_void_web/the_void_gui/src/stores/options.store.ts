import { defineStore } from "pinia";

export const useOptionStorage = defineStore("optionStorage", {
  state: () => ({
    option: "" as string,
  }),
  actions: {
    set(value: string) {
      this.option = value;
    },
  },
  persist: true,
});
