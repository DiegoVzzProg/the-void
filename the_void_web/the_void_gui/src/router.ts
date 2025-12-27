import {
  createRouter,
  createWebHistory,
  type RouteRecordRaw,
} from "vue-router";

import HomeView from "./views/HomeView.vue";

const routes: Array<RouteRecordRaw> = [
  { path: "/:pathMatch(.*)*", redirect: { name: "home" } },
  { path: "/", name: "home", component: HomeView, meta: { layout: "Main" } },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

router.beforeEach(async (_to, _from, next) => {
  return next();
});

export default router;
