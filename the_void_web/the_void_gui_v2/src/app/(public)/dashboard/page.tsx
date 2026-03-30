"use client";

import { Download, Logs, UsersRound } from "lucide-react";
import CardInformation from "@/shared/components/common/CardInformation";

export default function PanelPage() {
    return (
        <>
            <div className="grid grid-cols-1 md:grid-cols-2 xl:grid-cols-3 gap-4">
                <CardInformation>
                    <div className="flex items-center gap-5">
                        <div
                            className="flex size-10 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                            <UsersRound className="size-5 text-indigo-400" />
                        </div>
                        <div>
                            <h3 className="text-lg font-bold text-white mb-1 select-none">Total Users</h3>
                            <p className="text-sm leading-relaxed text-gray-400 select-none">
                                0
                            </p>
                        </div>
                    </div>
                </CardInformation>
                <CardInformation>
                    <div className="flex items-center gap-5">
                        <div
                            className="flex size-10 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                            <Download className="size-5 text-indigo-400" />
                        </div>
                        <div>
                            <h3 className="text-lg font-bold text-white mb-1 select-none">Total Downloads</h3>
                            <p className="text-sm leading-relaxed text-gray-400 select-none">
                                0
                            </p>
                        </div>
                    </div>
                </CardInformation>
                <CardInformation>
                    <div className="flex items-center gap-5">
                        <div
                            className="flex size-10 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                            <Logs className="size-5 text-indigo-400" />
                        </div>
                        <div>
                            <h3 className="text-lg font-bold text-white mb-1 select-none">Logs</h3>
                            <p className="text-sm leading-relaxed text-gray-400 select-none">
                                0
                            </p>
                        </div>
                    </div>
                </CardInformation>
            </div>
        </>
    );
}