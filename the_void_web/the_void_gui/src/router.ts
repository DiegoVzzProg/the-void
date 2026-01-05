import {
  createRouter,
  createWebHistory,
  type RouteRecordRaw,
} from "vue-router";

import WelcomeView from "./views/options/WelcomeView.vue";
import AuthView from "./views/AuthView.vue";
import ModulesView from "./views/options/ModulesView.vue";

const routes: Array<RouteRecordRaw> = [
  { path: "/:pathMatch(.*)*", redirect: { name: "information" } },
  {
    path: "/information",
    name: "information",
    component: WelcomeView,
    meta: { layout: "Main" },
  },
  {
    path: "/auth/:view",
    name: "auth",
    component: AuthView,
    meta: { layout: "Main" },
  },
  {
    path: "/modules",
    name: "modules",
    component: ModulesView,
    meta: { layout: "Main" },
  },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

router.beforeEach(async (_to, _from, next) => {
  return next();
});

export default router;
