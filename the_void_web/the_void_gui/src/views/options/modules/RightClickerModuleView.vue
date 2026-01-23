<script setup lang="ts">
import { ref, watch } from 'vue';
import { useDebounceFn } from '@vueuse/core';
import { CommandModuleService } from '../../../services/command-module.service';
import type { ICommand } from '../../../interfaces/command-module.interface';
import type { IRightClicker } from '../../../interfaces/modules.interface';
import type { SelectOption } from '../../../components/SelectOptions.vue';

import CardModule from '../../../components/CardModule.vue';
import Slider from '../../../components/Slider.vue';
import SelectOptions from '../../../components/SelectOptions.vue';

const props = defineProps<{ settings: IRightClicker | null }>();

// Constants
const RIGHT_CLICKER_OPTIONS: SelectOption[] = [
    { label: "blatan", value: "blatan" },
    { label: "exhaust", value: "exhaust" },
    { label: "bridge logic", value: "bridge_logic" }
];

const MIN_CPS = 5;
const NORMAL_MAX_CPS = 24;
const BLATANT_MAX_CPS = 80;

// State
const rightClicker = ref<boolean>(false);
const rightClickerValue = ref<number>(12);
const rightClickerConditions = ref<string[]>([]);
const rightClickerSliderMin = ref<number>(MIN_CPS);
const rightClickerSliderMax = ref<number>(NORMAL_MAX_CPS);
const rightClickerValuesBlackList = ref<string[]>([]);

// Services
const commandService = new CommandModuleService();

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

    rightClicker.value = settings.action === 'enable';
    rightClickerValue.value = settings.cps;
    rightClickerConditions.value = settings.conditions;

    // Sync slider max if blatan is enabled
    if (rightClickerConditions.value.includes("blatan")) {
        rightClickerSliderMax.value = BLATANT_MAX_CPS;
    }
}

// Logic
const commandModule = async (commandParams: ICommand) => {
    try {
        const command = await commandService.Command(commandParams);
        const data = command.data;
        if (!data || data.status !== 'Connected') return;
    } catch (error) {
        console.error("Error sending command:", error);
    }
}

const rightClickerModule = async () => {
    const commandParams: ICommand = {
        module: "right-clicker",
        action: rightClicker.value ? "enable" : "disable",
        settings: {
            cps: rightClickerValue.value
        },
        conditions: rightClickerConditions.value
    }

    await commandModule(commandParams);
}

// Handlers
const onActiveRightClicker = async (value: boolean) => {
    rightClicker.value = value;
    await rightClickerModule();
}

// Optimized: Debounced execution for slider changes
const debouncedRightClickerModule = useDebounceFn(async () => {
    await rightClickerModule();
}, 300);

const onChangeRightClickerValue = () => {
    if (!rightClicker.value) return;
    debouncedRightClickerModule();
}

const onChangeRightClickerConditions = async (values: string[]) => {
    if (!rightClicker.value) return;

    if (values.includes("blatan")) {
        rightClickerSliderMax.value = BLATANT_MAX_CPS;
    } else if (!values.includes("blatan") && rightClickerSliderMax.value === BLATANT_MAX_CPS) {
        rightClickerSliderMax.value = NORMAL_MAX_CPS;
        // Clamp value if needed
        if (rightClickerValue.value > NORMAL_MAX_CPS) {
            rightClickerValue.value = 12; // Reset to default safe value
        }
    }

    await rightClickerModule();
}
</script>

<template>
    <CardModule module-text="right clicker" icon="Mouse" :active-module="rightClicker"
        v-on:module-active="onActiveRightClicker">
        <div class="space-y-6">
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>CPS</span>
                    <span class="text-white">{{ rightClickerValue }}</span>
                </div>
                <Slider :min="rightClickerSliderMin" :max="rightClickerSliderMax" v-model="rightClickerValue"
                    v-on:change="onChangeRightClickerValue" />
            </div>
            <SelectOptions v-model="rightClickerConditions" :options="RIGHT_CLICKER_OPTIONS"
                :value-black-list="rightClickerValuesBlackList" v-on:change="onChangeRightClickerConditions" />
        </div>
    </CardModule>
</template>

<style scoped></style>
