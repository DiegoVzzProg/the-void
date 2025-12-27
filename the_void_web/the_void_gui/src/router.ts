import {
  createRouter,
  createWebHistory,
  type RouteRecordRaw,
} from "vue-router";

import HomeView from "./views/HomeView.vue";
import PanelView from "./views/PanelView.vue";

const routes: Array<RouteRecordRaw> = [
  { path: "/:pathMatch(.*)*", redirect: { name: "home" } },
  { path: "/", name: "home", component: HomeView, meta: { layout: "Main" } },
  {
    path: "/panel",
    name: "panel",
    component: PanelView,
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
