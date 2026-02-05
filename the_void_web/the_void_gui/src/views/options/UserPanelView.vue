<script setup lang="ts">
import { AlertTriangle, LogOut } from 'lucide-vue-next';
import { useRouter } from 'vue-router';
import { useAuthStorage } from '../../stores/auth.store';
import type { ICommand } from '../../interfaces/command-module.interface';
import { CommandModuleService } from '../../services/command-module.service';

const commandService = new CommandModuleService();
const router = useRouter();
const authStore = useAuthStorage();

const commandModule = async (commandParams: ICommand) => {
    try {
        const command = await commandService.Command(commandParams);
        const data = command.data;
        if (!data || data.status !== 'Connected') return;
    } catch (error) {
        console.error("Error sending command:", error);
    }
}

const handlePanic = async () => {
    const commandParams: ICommand = {
        module: "panic",
        action: "enable",
        settings: {},
    }

    await commandModule(commandParams);
};

const handleLogout = () => {
    authStore.deleteToken("__voidAccessToken");
    authStore.deleteToken("__voidRefreshToken");
    authStore.deactivateOption("no");
    router.push('/auth/login');
};
</script>

<template>
    <div class="flex flex-col h-full overflow-y-auto overflow-x-hidden pr-2">
        <header class="w-full min-h-16 flex items-center mb-6 shrink-0">
            <h1 class="text-3xl font-bold text-white tracking-tight flex items-center gap-3">
                <span
                    class="w-1.5 h-8 bg-linear-to-b from-indigo-500 to-purple-600 rounded-full shadow-[0_0_15px_rgba(99,102,241,0.5)]"></span>
                <span class="bg-clip-text text-transparent bg-linear-to-r from-white to-gray-400">user panel</span>
            </h1>
        </header>

        <div
            class="flex-1 flex flex-col items-center justify-center p-8 bg-[#0a0a0c]/40 rounded-3xl border border-white/5 backdrop-blur-sm">
            <div class="w-full max-w-md flex flex-col gap-6">
                <!-- Panic Button -->
                <button @click="handlePanic"
                    class="group relative w-full h-32 flex flex-col items-center justify-center gap-3 rounded-2xl bg-red-500/5 hover:bg-red-500/10 border border-red-500/10 hover:border-red-500/30 transition-all duration-300 active:scale-[0.98] shadow-lg shadow-red-900/5 hover:shadow-red-500/10 overflow-hidden">
                    <!-- Pulse Effect -->
                    <div
                        class="absolute inset-0 bg-linear-to-r from-transparent via-red-500/5 to-transparent -translate-x-full group-hover:animate-[shimmer_2s_infinite]">
                    </div>

                    <div
                        class="p-4 rounded-full bg-red-500/10 border border-red-500/20 group-hover:bg-red-500/20 group-hover:scale-110 transition-all duration-300">
                        <AlertTriangle
                            class="w-8 h-8 text-red-400 group-hover:text-red-300 drop-shadow-[0_0_8px_rgba(248,113,113,0.5)]" />
                    </div>

                    <div class="text-center z-10">
                        <span
                            class="text-xl font-bold text-red-400 group-hover:text-red-300 tracking-wider transition-colors">PANIC</span>
                        <p
                            class="text-xs text-red-500/50 mt-1 font-mono uppercase tracking-widest group-hover:text-red-400/70 transition-colors">
                            Emergency Protocol</p>
                    </div>
                </button>

                <!-- Logout Button -->
                <button @click="handleLogout"
                    class="w-full h-14 flex items-center justify-center gap-3 rounded-xl border border-white/5 bg-white/2 hover:bg-white/5 hover:border-indigo-500/30 transition-all duration-300 group">
                    <LogOut class="w-5 h-5 text-gray-400 group-hover:text-indigo-400 transition-colors" />
                    <span class="text-gray-400 group-hover:text-indigo-400 font-medium transition-colors">Logout</span>
                </button>
            </div>
        </div>
    </div>
</template>

<style scoped>
@keyframes shimmer {
    100% {
        transform: translateX(100%);
    }
}
</style>
