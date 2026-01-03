import { createApp } from "vue";
import "./style.css";
import App from "./App.vue";
import router from "./router";
import { plugin, defaultConfig } from "@formkit/vue";
import config from "./formkit.config";
// 1. Importar Pinia y el Plugin
import { createPinia } from "pinia";
import piniaPluginPersistedstate from "pinia-plugin-persistedstate";

const app = createApp(App);
const pinia = createPinia();

// 2. Conectar el plugin a Pinia
pinia.use(piniaPluginPersistedstate);

app.use(plugin, defaultConfig(config));

app.use(router).mount("#app");
