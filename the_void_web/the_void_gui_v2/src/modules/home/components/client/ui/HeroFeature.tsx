"use client";

import { Axe, Crosshair, Mouse } from "lucide-react";

export default function HeroFeature() {
    return (
        <>
            <div
                className="relative w-full max-md:overflow-hidden rounded-[32px] bg-[#0c0e16] border border-indigo-500/20 shadow-2xl max-md:h-dvh h-full">

                <div
                    className="absolute inset-0 bg-[url('data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNDAiIGhlaWdodD0iNDAiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+PGNpcmNsZSBjeD0iMSIgY3k9IjEiIHI9IjEiIGZpbGw9InJnYmEoMjU1LDI1NSwyNTUsMC4wNSkiLz48L3N2Zz4=')] opacity-30 pointer-events-none">
                </div>

                <div className="relative z-10 grid lg:grid-cols-2 gap-12 p-8 md:p-12 lg:items-center">

                    <div className="flex flex-col items-start space-y-6">
                        <div
                            className="inline-flex items-center gap-2 rounded-full border border-indigo-500/30 bg-indigo-500/10 px-3 py-1 text-xs font-medium text-indigo-300 backdrop-blur-sm">
                            <span className="relative flex h-2 w-2">
                                <span
                                    className="animate-ping absolute inline-flex h-full w-full rounded-full bg-indigo-400 opacity-75"></span>
                                <span className="relative inline-flex rounded-full h-2 w-2 bg-indigo-500"></span>
                            </span>
                            System Undetected
                        </div>

                        <div className="space-y-2">
                            <h1 className="text-4xl md:text-5xl font-bold tracking-tight text-white leading-tight">
                                Ready to <br />
                                <span
                                    className="text-transparent bg-clip-text bg-linear-to-r from-indigo-400 to-purple-400">dominate?</span>
                            </h1>
                            <p className="text-lg md:text-xl text-gray-400 font-light">
                                Become what they cannot see.
                            </p>
                        </div>

                        <p className="max-w-md text-sm text-gray-500 leading-relaxed">
                            Enhance your gameplay with our state-of-the-art stealth modules.
                            Designed for performance, security, and total control.
                        </p>
                    </div>

                    <div className="relative flex justify-center lg:justify-end perspective-1000">

                        <div className="relative w-full max-w-[340px]">

                            <div
                                className="absolute -inset-1 rounded-2xl bg-linear-to-r from-indigo-500 to-purple-600 opacity-40 blur-2xl">
                            </div>

                            <div
                                className="relative overflow-hidden rounded-2xl border border-white/10 bg-[#13151f]/95 backdrop-blur-xl shadow-2xl">

                                <div className="flex items-center justify-between border-b border-white/5 bg-white/5 px-5 py-4">
                                    <span className="text-xs font-medium text-gray-400 tracking-wide">www.thevoid.com</span>
                                    <div className="flex gap-1.5">
                                        <div className="h-2.5 w-2.5 rounded-full bg-[#ef4444]"></div>
                                        <div className="h-2.5 w-2.5 rounded-full bg-[#eab308]"></div>
                                        <div className="h-2.5 w-2.5 rounded-full bg-[#22c55e]"></div>
                                    </div>
                                </div>

                                <div className="p-4 space-y-3">

                                    <div
                                        className="relative overflow-hidden rounded-xl bg-linear-to-r from-[#6366f1] to-[#8b5cf6] p-4 shadow-lg shadow-indigo-500/25 transition-transform hover:scale-[1.02]">
                                        <div className="flex items-center justify-between relative z-10">
                                            <div className="flex items-center gap-3">
                                                <div
                                                    className="flex h-10 w-10 items-center justify-center rounded-lg bg-white/20 text-white backdrop-blur-sm">
                                                    <Crosshair className="h-5 w-5" strokeWidth="2.5" />
                                                </div>
                                                <div>
                                                    <h4 className="text-sm font-bold text-white tracking-wide">AimAssist</h4>
                                                    <p className="text-[10px] font-medium text-indigo-100/80">Active • Smooth</p>
                                                </div>
                                            </div>

                                            <div
                                                className="h-6 w-10 rounded-full bg-white/20 p-1 flex items-center justify-end backdrop-blur-sm cursor-pointer">
                                                <div className="h-4 w-4 rounded-full bg-white shadow-sm"></div>
                                            </div>
                                        </div>
                                    </div>

                                    <div
                                        className="flex items-center justify-between rounded-xl bg-[#1e2029] p-4 border border-white/5">
                                        <div className="flex items-center gap-3">
                                            <div
                                                className="flex h-10 w-10 items-center justify-center rounded-lg bg-[#272a37] text-gray-500 border border-white/5">
                                                <Axe className="h-5 w-5" />
                                            </div>
                                            <div>
                                                <h4 className="text-sm font-medium text-gray-300">Reach</h4>
                                                <p className="text-[10px] text-gray-600">Range: 4.0</p>
                                            </div>
                                        </div>

                                        <div
                                            className="h-6 w-10 rounded-full border-2 border-zinc-700 p-0.5 flex items-center justify-start opacity-60">
                                            <div className="h-3 w-3 rounded-full border-2 border-zinc-600 ml-0.5"></div>
                                        </div>
                                    </div>

                                    <div
                                        className="flex items-center justify-between rounded-xl bg-[#1e2029] p-4 border border-white/5">
                                        <div className="flex items-center gap-3">
                                            <div
                                                className="flex h-10 w-10 items-center justify-center rounded-lg bg-[#272a37] text-gray-500 border border-white/5">
                                                <Mouse className="h-5 w-5" />
                                            </div>
                                            <div>
                                                <h4 className="text-sm font-medium text-gray-300">AutoClicker</h4>
                                                <p className="text-[10px] text-gray-600">CPS: 24</p>
                                            </div>
                                        </div>

                                        <div
                                            className="h-6 w-10 rounded-full border-2 border-zinc-700 p-0.5 flex items-center justify-start opacity-60">
                                            <div className="h-3 w-3 rounded-full border-2 border-zinc-600 ml-0.5"></div>
                                        </div>
                                    </div>

                                </div>

                                <div
                                    className="mt-2 flex justify-between border-t border-white/5 bg-[#0a0a0c]/30 px-5 py-3 text-[10px] font-medium text-gray-600">
                                    <span>web</span>
                                    <span>v1.0</span>
                                </div>

                            </div>

                            <div
                                className="absolute -right-5 top-10 -z-10 h-full w-full rounded-2xl border border-white/5 bg-[#1a1d2d]/40 backdrop-blur-[2px] transform rotate-3 scale-95 opacity-50">
                            </div>
                        </div>
                    </div>

                </div>
            </div>
        </>
    );
}