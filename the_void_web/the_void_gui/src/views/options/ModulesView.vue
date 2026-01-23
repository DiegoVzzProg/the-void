<script setup lang="ts">

import AutoClickerModuleView from './modules/AutoClickerModuleView.vue';
import { onMounted, ref } from 'vue';
import { CommandModuleService } from '../../services/command-module.service';
import type { Action, IAimAssist, IAutoClicker, IFirstHit, IReach, IRightClicker } from '../../interfaces/modules.interface';
// import AimAssistModuleView from './modules/AimAssistModuleView.vue';
// import ReachModuleView from './modules/ReachModuleView.vue';
// import FirstHitModuleView from './modules/FirstHitModuleView.vue';
// import VelocityModuleView from './modules/VelocityModuleView.vue';
import RightClickerModuleView from './modules/RightClickerModuleView.vue';
import LoadingModules from '../../components/LoadingModules.vue';

const defaultAutoClicker = ref<IAutoClicker | null>(null);
const defaultReach = ref<IReach | null>(null);
const defaultFirstHit = ref<IFirstHit | null>(null);
const defaultAimAssist = ref<IAimAssist | null>(null);
const defaultRightClicker = ref<IRightClicker | null>(null);

const isLoading = ref(true);

onMounted(async () => {
    await loadConfigModules();
});

const loadConfigModules = async () => {
    // Artificial delay to show the Kernel Boot sequence
    const delayPromise = new Promise(resolve => setTimeout(resolve, 2500));
    const configPromise = new CommandModuleService().loadConfig();

    const [_, res] = await Promise.all([delayPromise, configPromise]);

    if (!res.data)
        return;

    const data = res.data;
    for (let i = 0; i < data.length; i++) {
        const action: Action = data[i]?.action ?? "disable";
        const settings: Record<string, any> | null = data[i]?.settings ?? null;
        const conditions: string[] = data[i]?.conditions ?? [];

        switch (data[i]?.module) {
            case "auto-clicker":
                defaultAutoClicker.value = {
                    action: action,
                    cps: Number(settings?.cps) ?? 12,
                    conditions: conditions ?? []
                };
                break;
            case "right-clicker":
                defaultRightClicker.value = {
                    action: action,
                    cps: Number(settings?.cps) ?? 12,
                    conditions: conditions ?? []
                };
                break;
            case "reach":
                defaultReach.value = {
                    action: action,
                    range: Number(settings?.range) ?? 3.00,
                    hitchance: Number(settings?.hitchance) ?? 90,
                    conditions: conditions
                }
                break;
            case "first-hit":
                defaultFirstHit.value = {
                    action: action,
                    range: Number(settings?.range) ?? 0.00,
                }
                break;
            case "aim-assists":
                defaultAimAssist.value = {
                    action: action,
                    range: Number(settings?.range) ?? 3.00,
                    fov: Number(settings?.fov) ?? 35,
                    smoothness: Number(settings?.smoothness) ?? 18,
                    strafe_assist_value: Number(settings?.strafe_assist_value) ?? 3,
                    conditions: conditions
                }
                break;
        }
    }

    isLoading.value = false;
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
            <LoadingModules v-if="isLoading" />

            <div v-else class="grid grid-cols-1 md:grid-cols-2 gap-4 lg:gap-6 pb-10 overflow-auto h-full pr-2">
                <AutoClickerModuleView :settings="defaultAutoClicker ?? null" />
                <RightClickerModuleView :settings="defaultRightClicker ?? null" />
            </div>
        </Transition>
    </div>
</template>

<style scoped></style>