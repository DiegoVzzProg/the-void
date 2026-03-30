"use client"

import Logo from "@/shared/components/common/Logo";
import { CreditCard, LayoutDashboard, UserRoundSearch, Package, Settings, List } from "lucide-react";
import { usePathname, useRouter } from "next/navigation";

const iconMap = {
    LayoutDashboard,
    Package,
    List,
    UserRoundSearch
};

type IconName = keyof typeof iconMap;


export default function Sidebar() {
    const pathname = usePathname();
    const route = useRouter();

    const options = [
        { id: crypto.randomUUID(), icono: "LayoutDashboard", text: "Dashboard", visible: true, path: "/system/dashboard" },
        { id: crypto.randomUUID(), icono: "UserRoundSearch", text: "Users", visible: true, path: "/system/users" },
        { id: crypto.randomUUID(), icono: "List", text: "Options", visible: true, path: "/system/options" },
    ];


    const handleNavegation = (path: string) => {
        route.push(path);
    }

    return (
        <aside className="w-full max-w-xs h-full p-4 flex flex-col gap-5">
            <div className="flex w-full justify-center">
                <Logo css="size-[120px] select-none" />
            </div>
            <nav className="flex flex-col gap-2 w-full">
                {options.map((item) => {
                    if (!item.visible) return null;

                    const IconComponent = iconMap[item.icono as IconName];
                    const isActive = pathname === item.path;

                    return (
                        <button
                            key={item.id} onClick={() => { handleNavegation(item.path) }}
                            className={`w-full flex items-center p-3 rounded-xl transition-all duration-300 gap-3 group relative overflow-hidden 
                                ${isActive
                                    ? 'bg-indigo-500/10 text-indigo-400 font-medium'
                                    : 'hover:bg-white/5 hover:text-white text-slate-400'}`}>
                            {IconComponent && (
                                <IconComponent
                                    className={`transition-all duration-300 ${isActive ? 'size-[22px] stroke-[2.5px]' : 'size-[22px] stroke-[1.5px] group-hover:stroke-white'}`}
                                />
                            )}

                            <span className="max-lg:hidden text-sm tracking-wide">
                                {item.text}
                            </span>

                            {/* Active indicator strip */}
                            {isActive && (
                                <div className="absolute left-0 top-1/2 -translate-y-1/2 w-1 h-8 bg-indigo-500 rounded-r-full shadow-[0_0_10px_#6366f1] max-lg:hidden" />
                            )}
                        </button>
                    )
                })}
            </nav>
        </aside>
    );
}