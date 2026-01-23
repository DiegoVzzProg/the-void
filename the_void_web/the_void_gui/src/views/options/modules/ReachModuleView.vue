<script setup lang="ts">
import { ref, watch } from 'vue';
import { useDebounceFn } from '@vueuse/core';
import CardModule from '../../../components/CardModule.vue';
import SelectOptions, { type SelectOption } from '../../../components/SelectOptions.vue';
import Slider from '../../../components/Slider.vue';
import type { ICommand } from '../../../interfaces/command-module.interface';
import { CommandModuleService } from '../../../services/command-module.service';
import type { IReach } from '../../../interfaces/modules.interface';

const props = defineProps<{ settings: IReach | null }>();

const REACH_OPTIONS: SelectOption[] = [
    { label: "randomization", value: "randomization" },
    { label: "sprint only", value: "sprint_only" },
    { label: "hitboxes", value: "hitboxes" },
    { label: "visible only", value: "visible_only" }
];

const commandService = new CommandModuleService();

// State
const reach = ref<boolean>(false);
const reachRange = ref<number>(3.00);
const reachRangeSlider = ref<number>(300);
const reachRangeText = ref<string>("3.00");
const reachHitChance = ref<number>(90);

const reachConditionsSelected = ref<string[]>([]);
const reachValuesBlackList = ref<string[]>([]);

// Watchers
watch(
    () => props.settings,
    (newSettings) => {
        if (newSettings) loadConfig();
    },
    { immediate: true }
);

const loadConfig = () => {
    const settings = props.settings;
    if (!settings) return;

    reach.value = settings.action === 'enable';
    reachRange.value = settings.range;

    // Sync slider and text
    reachRangeSlider.value = settings.range * 100;
    reachRangeText.value = settings.range.toFixed(2);

    reachHitChance.value = settings.hitchance;
    reachConditionsSelected.value = settings.conditions;
}

// Logic
const commandModule = async (commandParams: ICommand) => {
    try {
        const res = await commandService.Command(commandParams);
        if (res.data && res.data.status !== "Connected") {
            // Error handling
        }
    } catch (e) {
        console.error("Command error", e);
    }
}

const reachModule = async () => {
    const commandParams: ICommand = {
        module: "reach",
        action: reach.value ? "enable" : "disable",
        settings: {
            range: reachRange.value,
            hitchance: reachHitChance.value
        },
        conditions: reachConditionsSelected.value
    }

    await commandModule(commandParams);
}

// Handlers
const onActiveReach = async (value: boolean) => {
    reach.value = value;
    await reachModule();
}

// Debounce
const debouncedUpdate = useDebounceFn(async () => {
    if (!reach.value) return;
    await reachModule();
}, 300);

const onUpdateReachRange = (value: number) => {
    const fixedReach = (value / 100).toFixed(2);
    reachRangeText.value = fixedReach;
    reachRange.value = Number(fixedReach);
    reachRangeSlider.value = value;

    debouncedUpdate();
}

const onChangeReachRange = () => {
    debouncedUpdate();
}

const onChangeHitChance = () => {
    debouncedUpdate();
}

const onChangeConditionsReachRange = async (conditions: string[]) => {
    reachConditionsSelected.value = conditions;
    if (!reach.value) return;
    await reachModule();
}

</script>

<template>
    <CardModule module-text="Reach" icon="Axe" :active-module="reach" v-on:module-active="onActiveReach"
        module-info="Prioritize safety over raw advantage. This module varies your attack distance randomly on each hit using a human probability curve. Objective: Mimic natural imperfection to evade long-term bans on primary accounts.">
        <div class="space-y-6">
            <!-- Range -->
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Range</span>
                    <span class="text-white bg-white/5 px-2 py-0.5 rounded font-mono">{{ reachRangeText }}</span>
                </div>
                <!-- Note: max=600 implies 6.00 range logic -->
                <Slider :min="300" :max="600" v-model="reachRangeSlider" v-on:update:model-value="onUpdateReachRange"
                    v-on:change="onChangeReachRange" />
            </div>

            <!-- Hit Chance -->
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Hit Chance</span>
                    <span class="text-white bg-white/5 px-2 py-0.5 rounded font-mono">{{ reachHitChance }}%</span>
                </div>
                <Slider :min="0" :max="100" v-model="reachHitChance" v-on:change="onChangeHitChance" />
            </div>

            <!-- Options -->
            <div class="pt-2 border-t border-white/5">
                <SelectOptions v-model="reachConditionsSelected" :options="REACH_OPTIONS"
                    :value-black-list="reachValuesBlackList" v-on:change="onChangeConditionsReachRange" />
            </div>
        </div>
    </CardModule>
</template>

<style scoped></style>