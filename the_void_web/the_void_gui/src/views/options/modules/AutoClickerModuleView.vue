<script setup lang="ts">
import { ref, watch } from 'vue';
import { useDebounceFn } from '@vueuse/core';
import { CommandModuleService } from '../../../services/command-module.service';
import type { ICommand } from '../../../interfaces/command-module.interface';
import type { IAutoClicker } from '../../../interfaces/modules.interface';
import type { SelectOption } from '../../../components/SelectOptions.vue';

import CardModule from '../../../components/CardModule.vue';
import Slider from '../../../components/Slider.vue';
import SelectOptions from '../../../components/SelectOptions.vue';

const props = defineProps<{ settings: IAutoClicker | null }>();

// Constants
const AUTO_CLICKER_OPTIONS: SelectOption[] = [
    { label: "inventory", value: "inventory" },
    { label: "auto block hit", value: "auto_block_hit" },
    { label: "weapon check", value: "weapon_check" },
    { label: "ignore blocks", value: "ignore_blocks" },
];

const MIN_CPS = 10;
const NORMAL_MAX_CPS = 24;
// const BLATANT_MAX_CPS = 80;

// State
const autoClicker = ref<boolean>(false);
const autoClickerValue = ref<number>(12);
const autoClickerConditions = ref<string[]>([]);
const autoClickerSliderMin = ref<number>(MIN_CPS);
const autoClickerSliderMax = ref<number>(NORMAL_MAX_CPS);
const autoClickerValuesBlackList = ref<string[]>([]);

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

    autoClicker.value = settings.action === 'enable';
    autoClickerValue.value = settings.cps;
    autoClickerConditions.value = settings.conditions;

    // // Sync slider max if blatan is enabled
    // if (autoClickerConditions.value.includes("blatan")) {
    //     autoClickerSliderMax.value = BLATANT_MAX_CPS;
    // }
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

const autoClickerModule = async () => {
    const commandParams: ICommand = {
        module: "auto-clicker",
        action: autoClicker.value ? "enable" : "disable",
        settings: {
            cps: autoClickerValue.value
        },
        conditions: autoClickerConditions.value
    }

    await commandModule(commandParams);
}

// Handlers
const onActiveAutoClicker = async (value: boolean) => {
    autoClicker.value = value;
    await autoClickerModule();
}

// Optimized: Debounced execution for slider changes
const debouncedAutoClickerModule = useDebounceFn(async () => {
    await autoClickerModule();
}, 300);

const onChangeAutoClickerValue = () => {
    if (!autoClicker.value) return;
    debouncedAutoClickerModule();
}

const onChangeAutoClickerConditions = async (_values: string[]) => {
    if (!autoClicker.value) return;

    // if (values.includes("blatan")) {
    //     autoClickerSliderMax.value = BLATANT_MAX_CPS;
    // } else if (!values.includes("blatan") && autoClickerSliderMax.value === BLATANT_MAX_CPS) {
    //     autoClickerSliderMax.value = NORMAL_MAX_CPS;
    //     // Clamp value if needed
    //     if (autoClickerValue.value > NORMAL_MAX_CPS) {
    //         autoClickerValue.value = 12; // Reset to default safe value
    //     }
    // }

    await autoClickerModule();
}
</script>

<template>
    <CardModule module-text="left clicker" icon="Mouse" :active-module="autoClicker"
        v-on:module-active="onActiveAutoClicker">
        <div class="space-y-6">
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>CPS</span>
                    <span class="text-white bg-white/5 px-2 py-0.5 rounded font-mono">{{ autoClickerValue }}</span>
                </div>
                <Slider :min="autoClickerSliderMin" :max="autoClickerSliderMax" v-model="autoClickerValue"
                    v-on:change="onChangeAutoClickerValue" />
            </div>
            <SelectOptions v-model="autoClickerConditions" :options="AUTO_CLICKER_OPTIONS"
                :value-black-list="autoClickerValuesBlackList" v-on:change="onChangeAutoClickerConditions" />
        </div>
    </CardModule>
</template>

<style scoped></style>