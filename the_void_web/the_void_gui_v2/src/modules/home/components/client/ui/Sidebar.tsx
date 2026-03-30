'use client';

import { useMemo } from 'react';
import { usePathname, useRouter } from 'next/navigation';
import {
    LayoutDashboard, Package, Settings2,
    TvMinimal, Form, KeyRound, User,
    Shield
} from 'lucide-react';
import Logo from '@/shared/components/common/Logo';
import { useUserStore } from '@/modules/users';
import { IUser } from '@/modules/users';



const iconMap = {
    LayoutDashboard,
    Package,
    Settings2,
    TvMinimal,
    Form,
    KeyRound,
    Shield
};

type IconName = keyof typeof iconMap;

interface SidebarProps {
    serverUser?: IUser | null;
}

export default function Sidebar({ serverUser }: SidebarProps) {
    const pathname = usePathname();
    const router = useRouter();

    const storeUser = useUserStore(state => state.user);
    const storeIsAuthenticated = useUserStore(state => state.isAuthenticated);

    // El serverUser elimina el flash: en el primer render ya sabemos si hay sesión.
    // Después de la hidratación, Zustand toma el control reactivamente.
    const user = storeUser ?? serverUser ?? null;
    const isAuthenticated = storeIsAuthenticated || !!serverUser;

    const options = useMemo(() => [
        { id: 2, icono: "TvMinimal", text: "Home", visible: true, path: "/" },
        { id: 3, icono: "KeyRound", text: "auth", visible: !isAuthenticated, path: "/auth" },
        { id: 4, icono: "LayoutDashboard", text: "dashboard", visible: false, path: "/dashboard" },

    ], [isAuthenticated]);

    const handleNavigation = (path: string) => {
        router.push(path);
    };

    const userAction = () => {
        if (!isAuthenticated) {
            router.push("/auth");
            return;
        }
        router.push("/user");
    };
    return (
        <aside className="flex flex-row lg:flex-col bg-[#0e1012] w-full lg:max-w-[280px] h-full max-lg:max-h-[80px] lg:border-r lg:border-r-[#232732] gap-4 p-4 shrink-0 justify-between">

            <div className="flex flex-col gap-6 w-full">
                <div className="flex items-center justify-center px-4 max-lg:hidden">
                    <Logo css="size-[100px]" />
                </div>

                <nav className="flex lg:flex-col max-lg:justify-between gap-2 w-full">
                    {options.map((item) => {
                        if (!item.visible) return null;

                        const IconComponent = iconMap[item.icono as IconName];
                        const isActive = pathname === item.path;

                        return (
                            <button
                                key={item.id}
                                onClick={() => handleNavigation(item.path)}
                                className={`lg:w-full flex items-center p-3 rounded-xl transition-all duration-300 gap-3 group relative overflow-hidden ${isActive
                                    ? 'bg-indigo-500/10 text-indigo-400 font-medium'
                                    : 'hover:bg-white/5 hover:text-white text-slate-400'}`}
                            >
                                {IconComponent && (
                                    <IconComponent
                                        className={`transition-all duration-300 ${isActive ? 'size-[22px] stroke-[2.5px]' : 'size-[22px] stroke-[1.5px] group-hover:stroke-white'}`}
                                    />
                                )}

                                <span className="max-lg:hidden text-sm tracking-wide lowercase">
                                    {item.text}
                                </span>

                                {/* Active indicator strip */}
                                {isActive && (
                                    <div className="absolute left-0 top-1/2 -translate-y-1/2 w-1 h-8 bg-indigo-500 rounded-r-full shadow-[0_0_10px_#6366f1] max-lg:hidden" />
                                )}
                            </button>
                        );
                    })}
                </nav>
            </div>

            <div className='bg-[#18181B]/50 border border-[#232732] rounded-2xl p-3 lg:p-4 w-full flex flex-row lg:flex-row items-center gap-3 backdrop-blur-md transition-all hover:bg-[#18181B] hover:border-indigo-500/30 group cursor-pointer max-lg:hidden' onClick={userAction}>
                <div className='size-10 rounded-full overflow-hidden border border-indigo-500/50 bg-[#222] flex items-center justify-center shrink-0 shadow-[0_0_15px_-5px_#6366f1]'>
                    <User className='size-6 text-indigo-400' />
                </div>
                <div className="flex flex-col overflow-hidden">
                    <span className="text-sm font-semibold text-white truncate group-hover:text-indigo-300 transition-colors">
                        {user?.username ?? "username"}
                    </span>
                    <small className="text-xs text-slate-500 truncate">{user?.email ?? "email"}</small>
                </div>
            </div>
        </aside>
    );
}