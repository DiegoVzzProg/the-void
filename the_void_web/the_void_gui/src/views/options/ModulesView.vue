<script setup lang="ts">

import AutoClickerModuleView from './modules/AutoClickerModuleView.vue';
import ReachModuleView from './modules/ReachModuleView.vue';
import AimAssistModuleView from './modules/AimAssistModuleView.vue';
import VelocityModuleView from './modules/VelocityModuleView.vue';
import FirstHitModuleView from './modules/FirstHitModuleView.vue';
import { onMounted, ref } from 'vue';
import { CommandModuleService } from '../../services/command-module.service';
import type { Action, IAimAssist, IAutoClicker, IFirstHit, IReach } from '../../interfaces/modules.interface';

const defaultAutoClicker = ref<IAutoClicker | null>(null);
const defaultReach = ref<IReach | null>(null);
const defaultFirstHit = ref<IFirstHit | null>(null);
const defaultAimAssist = ref<IAimAssist | null>(null);

onMounted(async () => {
    await loadConfigModules();
});

const loadConfigModules = async () => {
    const res = await new CommandModuleService().loadConfig();

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

    console.log(defaultAutoClicker.value)
}

</script>

<template>
    <header class="w-full min-h-16 flex items-center mb-3">
        <h1 class="text-2xl font-bold text-white tracking-tight flex items-center gap-2">
            <span class="w-2 h-6 bg-[#6366f1] rounded-full"></span>
            modules
        </h1>
    </header>

    <div class="grid grid-cols-1 md:grid-cols-2 gap-4 lg:gap-6 pb-10 overflow-auto h-[calc(100dvh-186px)]">
        <AutoClickerModuleView :settings="defaultAutoClicker ?? null" />

        <AimAssistModuleView :settings="defaultAimAssist" />

        <ReachModuleView :settings="defaultReach" />

        <FirstHitModuleView :settings="defaultFirstHit" />

        <VelocityModuleView />
    </div>
</template>

<style scoped></style>