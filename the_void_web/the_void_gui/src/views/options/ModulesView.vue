<script setup lang="ts">

import AutoClickerModuleView from './modules/AutoClickerModuleView.vue';
import { onMounted, ref } from 'vue';
import { CommandModuleService } from '../../services/command-module.service';
import type { IAimAssist, IAutoClicker, IFirstHit, IReach } from '../../interfaces/modules.interface';
import LoadingModules from '../../components/LoadingModules.vue';
import AimAssistModuleView from './modules/AimAssistModuleView.vue';
import FirstHitModuleView from './modules/FirstHitModuleView.vue';
import ReachModuleView from './modules/ReachModuleView.vue';

const defaultAutoClicker = ref<IAutoClicker | null>(null);
const defaultReach = ref<IReach | null>(null);
const defaultFirstHit = ref<IFirstHit | null>(null);
const defaultAimAssist = ref<IAimAssist | null>(null);

const isLoading = ref(true);
const connectionStatus = ref<'pending' | 'success' | 'error'>('pending');

onMounted(async () => {
    await loadConfigModules();
});

const loadConfigModules = async () => {
    // Artificial delay to show the Kernel Boot sequence
    const delayPromise = new Promise(resolve => setTimeout(resolve, 2000));
    const configPromise = new CommandModuleService().loadConfig();

    try {
        const [_, res] = await Promise.all([delayPromise, configPromise]);

        if (!res.data) {
            connectionStatus.value = 'error';
            return;
        }

        // const data = res.data;
        // Parse data... logic remains same but commented out in original file so leaving it as is
        // ...

        connectionStatus.value = 'success';
    } catch (error) {
        console.error("Failed to load modules config", error);
        connectionStatus.value = 'error';
    }
}

</script>

<template>
    <header class="w-full min-h-16 flex items-center mb-6 shrink-0">
        <h1 class="text-3xl font-bold text-white tracking-tight flex items-center gap-3">
            <span
                class="w-1.5 h-8 bg-linear-to-b from-indigo-500 to-purple-600 rounded-full shadow-[0_0_15px_rgba(99,102,241,0.5)]"></span>
            <span class="bg-clip-text text-transparent bg-linear-to-r from-white to-gray-400">modules</span>
        </h1>
    </header>
    <div class="flex-1 overflow-hidden relative">
        <Transition enter-active-class="transition duration-500 ease-out" enter-from-class="opacity-0 scale-95"
            enter-to-class="opacity-100 scale-100" leave-active-class="transition duration-300 ease-in"
            leave-from-class="opacity-100 scale-100" leave-to-class="opacity-0 scale-95" mode="out-in">
            <LoadingModules v-if="isLoading" :connection-status="connectionStatus" @complete="isLoading = false" />

            <div v-else class="grid grid-cols-1 md:grid-cols-2 gap-4 lg:gap-6 pb-10 overflow-auto h-full pr-2">
                <AutoClickerModuleView :settings="defaultAutoClicker ?? null" />
                <ReachModuleView :settings="defaultReach ?? null" />
                <AimAssistModuleView :settings="defaultAimAssist ?? null" />
                <FirstHitModuleView :settings="defaultFirstHit ?? null" />
            </div>
        </Transition>
    </div>
</template>

<style scoped></style>