import { ReactNode } from "react";
import { toast } from "sonner";

export enum TypeToast {
    INFO = "INFO",
    ERROR = "ERROR",
    SUCCESS = "SUCCESS",
    WARNING = "WARNING",
    LOADING = "LOADING"
}

export interface IToast {
    icon?: ReactNode;
    type: TypeToast;
    title: string;
    message: string;
}

export class Site {
    public static Toast(params: IToast) {
        switch (params.type) {
            case TypeToast.ERROR:
                toast.error(params.title, { description: params.message });
                break;
            case TypeToast.INFO:
                toast.info(params.title, { description: params.message });
                break;
            case TypeToast.SUCCESS:
                toast.success(params.title, { description: params.message });
                break;
            case TypeToast.LOADING:
                toast.loading(params.title, { description: params.message });
                break;
            case TypeToast.WARNING:
                toast.warning(params.title, { description: params.message });
                break;
        }
    }
}