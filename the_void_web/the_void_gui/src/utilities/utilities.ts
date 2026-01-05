import type { RouteLocationRaw, RouteRecordName } from "vue-router";
import router from "../router";

interface RouterParams {
  name: RouteRecordName;
  params?: Record<string, string>;
  functionOn?: () => void;
  query?: Record<string, string>;
}

export class Utils {
  public RedirectPage(name: string): void;
  public RedirectPage(routerParams: RouterParams): void;
  public RedirectPage(routerParams: RouterParams | string): void {
    let dataRoute: RouteLocationRaw;
    if (typeof routerParams === "string") {
      dataRoute = { name: routerParams };
    } else {
      dataRoute = {
        name: routerParams.name,
        params: routerParams.params ?? {},
        query: routerParams.query ?? {},
      };
    }

    router
      .push(dataRoute)
      .then(() => {
        window.scrollTo({ top: 0 });
        if (typeof routerParams !== "string") {
          routerParams.functionOn?.();
        }
      })
      .catch((error) => {
        if (error.name !== "NavigationDuplicated") {
          router.push("/").catch(() => {
            window.location.href = "/";
          });
        }
      });
  }
}
