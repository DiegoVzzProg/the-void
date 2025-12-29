<script setup lang="ts">
import {
    Crosshair,
    ChevronRight,
    ToggleRight,
    Axe,
    Mouse
} from 'lucide-vue-next';
</script>
<template>
    <div
        class="relative w-full overflow-hidden rounded-[32px] bg-[#0c0e16] border border-indigo-500/20 shadow-2xl h-auto lg:min-h-full">

        <div
            class="absolute -top-[20%] -left-[10%] h-[500px] w-[500px] rounded-full bg-indigo-600/20 blur-[120px] pointer-events-none">
        </div>

        <div
            class="absolute inset-0 bg-[url('data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNDAiIGhlaWdodD0iNDAiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+PGNpcmNsZSBjeD0iMSIgY3k9IjEiIHI9IjEiIGZpbGw9InJnYmEoMjU1LDI1NSwyNTUsMC4wNSkiLz48L3N2Zz4=')] opacity-30 pointer-events-none">
        </div>

        <div class="relative z-10 grid lg:grid-cols-2 gap-12 p-8 md:p-12 lg:items-center">

            <div class="flex flex-col items-start space-y-6">

                <div
                    class="inline-flex items-center gap-2 rounded-full border border-indigo-500/30 bg-indigo-500/10 px-3 py-1 text-xs font-medium text-indigo-300 backdrop-blur-sm">
                    <span class="relative flex h-2 w-2">
                        <span
                            class="animate-ping absolute inline-flex h-full w-full rounded-full bg-indigo-400 opacity-75"></span>
                        <span class="relative inline-flex rounded-full h-2 w-2 bg-indigo-500"></span>
                    </span>
                    System Undetected
                </div>

                <div class="space-y-2">
                    <h1 class="text-4xl md:text-5xl font-bold tracking-tight text-white leading-tight">
                        Ready to <br />
                        <span class="gradient-text">dominate?</span>
                    </h1>
                    <p class="text-lg md:text-xl text-gray-400 font-light">
                        Become what they cannot see.
                    </p>
                </div>

                <p class="max-w-md text-sm text-gray-500 leading-relaxed">
                    Enhance your gameplay with our state-of-the-art stealth modules.
                    Designed for performance, security, and total control.
                </p>

                <div class="flex flex-wrap gap-4 pt-2">
                    <button
                        class="group flex items-center gap-2 rounded-xl bg-indigo-600 px-6 py-3.5 text-sm font-semibold text-white shadow-[0_0_20px_-5px_rgba(79,70,229,0.5)] transition-all hover:bg-indigo-500 hover:shadow-[0_0_25px_-5px_rgba(79,70,229,0.7)] active:scale-95">
                        start injection
                        <ChevronRight class="h-4 w-4 transition-transform group-hover:translate-x-1" />
                    </button>
                </div>
            </div>

            <div class="relative flex justify-center lg:justify-end">

                <div class="relative w-full max-w-[320px]">

                    <div
                        class="absolute -inset-1 rounded-2xl bg-gradient-to-r from-indigo-500 to-purple-600 opacity-30 blur-xl">
                    </div>

                    <div
                        class="relative overflow-hidden rounded-2xl border border-white/10 bg-[#13151f]/90 backdrop-blur-xl shadow-2xl">

                        <div class="flex items-center justify-between border-b border-white/5 bg-white/5 px-4 py-3">
                            <span class="text-[14px] gradient-text">www.thevoid.com</span>
                            <div class="flex gap-1.5">
                                <div class="h-2 w-2 rounded-full bg-red-500/50"></div>
                                <div class="h-2 w-2 rounded-full bg-yellow-500/50"></div>
                                <div class="h-2 w-2 rounded-full bg-green-500/50"></div>
                            </div>
                        </div>

                        <div class="p-3 space-y-2">

                            <div
                                class="relative group overflow-hidden rounded-xl bg-gradient-to-r from-indigo-900/60 to-purple-900/60 border border-indigo-500/30 p-3 transition-all hover:border-indigo-400/50">
                                <div class="flex items-center justify-between relative z-10">
                                    <div class="flex items-center gap-3">
                                        <div
                                            class="flex h-8 w-8 items-center justify-center rounded-lg bg-indigo-500 text-white shadow-lg shadow-indigo-500/20">
                                            <Crosshair class="h-4 w-4" />
                                        </div>
                                        <div>
                                            <h4 class="text-sm font-bold text-white">AimAssist</h4>
                                            <p class="text-[10px] text-indigo-200">Active • Smooth</p>
                                        </div>
                                    </div>
                                    <div class="h-5 w-9 rounded-full bg-indigo-500 p-1">
                                        <div class="h-3 w-3 rounded-full bg-white shadow-sm translate-x-4"></div>
                                    </div>
                                </div>
                            </div>

                            <div
                                class="flex items-center justify-between rounded-xl bg-white/5 p-3 border border-transparent hover:bg-white/10 transition-colors">
                                <div class="flex items-center gap-3">
                                    <div
                                        class="flex h-8 w-8 items-center justify-center rounded-lg bg-[#1a1d2d] text-gray-400 border border-white/5">
                                        <Axe class="h-4 w-4" />
                                    </div>
                                    <div>
                                        <h4 class="text-sm font-medium text-gray-300">Reach</h4>
                                        <p class="text-[10px] text-gray-500">Range: 4.0</p>
                                    </div>
                                </div>
                                <ToggleRight class="h-5 w-5 text-gray-600" />
                            </div>

                            <div
                                class="flex items-center justify-between rounded-xl bg-white/5 p-3 border border-transparent hover:bg-white/10 transition-colors">
                                <div class="flex items-center gap-3">
                                    <div
                                        class="flex h-8 w-8 items-center justify-center rounded-lg bg-[#1a1d2d] text-gray-400 border border-white/5">
                                        <Mouse class="h-4 w-4" />
                                    </div>
                                    <div>
                                        <h4 class="text-sm font-medium text-gray-300">AutoClicker</h4>
                                        <p class="text-[10px] text-gray-500">CPS: 24</p>
                                    </div>
                                </div>
                                <ToggleRight class="h-5 w-5 text-gray-600" />
                            </div>

                        </div>

                        <div
                            class="mt-2 flex justify-between border-t border-white/5 bg-black/20 px-4 py-2 text-[10px] text-gray-500">
                            <span>web</span>
                            <span>v1.0</span>
                        </div>

                    </div>

                    <div
                        class="absolute -right-4 top-8 -z-10 h-full w-full rounded-2xl border border-white/5 bg-[#1a1d2d]/80 backdrop-blur-sm transform rotate-3 scale-95 opacity-60">
                    </div>

                </div>
            </div>

        </div>
    </div>
</template>

<style scoped>
@keyframes gradient-x {

    0%,
    100% {
        background-position: 0% 50%;
    }

    50% {
        background-position: 100% 50%;
    }
}

.animate-gradient {
    background-size: 200% 200%;
    animation: gradient-x 3s ease infinite;
}
</style>