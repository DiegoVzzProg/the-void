"use client";

import { usePathname } from "next/navigation";

export default function PageTitle() {
    const pathname = usePathname();

    const routeNames: Record<string, string> = {
        "/": "information",
        "/dashboard": "Dashboard",
        "/modules": "Modules",
        "/forum": "Forum",
        "/auth": "Auth",
    };

    const except: string[] = ["/auth", "/user"];

    const currentTitle: string = routeNames[pathname];
    return (
        <>
            {!except.includes(pathname) && <header className="w-full min-h-16 flex items-center mb-6 shrink-0">
                <h1 className="text-3xl font-bold text-white tracking-tight flex items-center gap-3">
                    <span
                        className="w-1.5 h-8 bg-linear-to-b from-indigo-500 to-purple-600 rounded-full shadow-[0_0_15px_rgba(99,102,241,0.5)]"></span>
                    <span
                        className="bg-clip-text text-transparent bg-linear-to-r from-white to-gray-400 lowercase select-none ">
                        {currentTitle}
                    </span>
                </h1>
            </header>}
        </>
    );
}