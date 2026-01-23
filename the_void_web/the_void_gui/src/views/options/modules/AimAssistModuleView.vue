<script setup lang="ts">
import { ref, watch } from 'vue';
import { useDebounceFn } from '@vueuse/core';
import CardModule from '../../../components/CardModule.vue';
import SelectOptions, { type SelectOption } from '../../../components/SelectOptions.vue';
import Slider from '../../../components/Slider.vue';
import { CommandModuleService } from '../../../services/command-module.service';
import type { ICommand } from '../../../interfaces/command-module.interface';
import type { IAimAssist } from '../../../interfaces/modules.interface';

const props = defineProps<{ settings: IAimAssist | null }>();

// Constants
const AIM_ASSIST_OPTIONS: SelectOption[] = [
    { label: "weapon check", value: "weapon_check" },
    { label: "break blocks", value: "break_blocks" },
    { label: "click only", value: "click_only" },
    { label: "visibility check", value: "visibility_check" },
    { label: "strafe assist", value: "strafe_assist" },
];

const commandService = new CommandModuleService();

// State
const aimAssists = ref<boolean>(false);
const aimAssistsFov = ref<number>(35);
const aimAssistsRange = ref<number>(4.0);
const aimAssistsRangeSlider = ref<number>(40);
const aimAssistsRangeText = ref<string>("4.0");
const aimAssistsSmooth = ref<number>(18);
const aimAssistsConditionsSelected = ref<string[]>([]);
const aimAssistsValuesBlackList = ref<string[]>([]);
const aimAssistsStrafe = ref<boolean>(false);
const aimAssistsStrafeValue = ref<number>(3);
const aimAssistsStrafeSlider = ref<number>(40);
const aimAssistsStrafeText = ref<string>("4.0");

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

    aimAssists.value = settings.action === 'enable';
    aimAssistsRange.value = settings.range;

    // Sync slider and text for range
    aimAssistsRangeSlider.value = settings.range * 10;
    aimAssistsRangeText.value = settings.range.toFixed(1);

    aimAssistsFov.value = settings.fov;
    aimAssistsSmooth.value = settings.smoothness;
    aimAssistsConditionsSelected.value = settings.conditions;
    aimAssistsStrafe.value = settings.conditions.includes("strafe-assist");

    // Sync strafe values if available in settings (assuming logic based entirely on existing code structure)
    if (settings.strafe_assist_value !== undefined) {
        aimAssistsStrafeValue.value = settings.strafe_assist_value;
        aimAssistsStrafeSlider.value = settings.strafe_assist_value * 10;
        aimAssistsStrafeText.value = settings.strafe_assist_value.toFixed(1);
    }
}

// Logic
const commandModule = async (commandParams: ICommand) => {
    try {
        const res = await commandService.Command(commandParams);
        // Error handling or status check logic
        if (res.data && res.data.status !== "ok") {
            // Handle error silently or log
        }
    } catch (e) {
        console.error("Command error", e);
    }
}

const activeAimAssistsModule = async () => {
    const commandParams: ICommand = {
        module: "aim-assists",
        action: aimAssists.value ? "enable" : "disable",
        settings: {
            fov: aimAssistsFov.value,
            smoothness: aimAssistsSmooth.value,
            range: aimAssistsRange.value,
            strafe_assist_value: aimAssistsStrafeValue.value
        },
        conditions: aimAssistsConditionsSelected.value
    }
    await commandModule(commandParams);
}

// Handlers
const onActiveAimAssists = async (value: boolean) => {
    aimAssists.value = value;
    await activeAimAssistsModule();
}

// Debounced Update
const debouncedUpdate = useDebounceFn(async () => {
    if (!aimAssists.value) return;
    await activeAimAssistsModule();
}, 300);

const onUpdateAimAssistsStrafe = (value: number) => {
    // Business logic: value / 10
    const fixedAimAssistsStrafe = (value / 10).toFixed(1);
    aimAssistsStrafeText.value = fixedAimAssistsStrafe;
    aimAssistsStrafeValue.value = Number(fixedAimAssistsStrafe);
    aimAssistsStrafeSlider.value = value;

    debouncedUpdate();
}

const onUpdateAimAssistsRange = (value: number) => {
    // Business logic: value / 10
    const fixedAimAssistsRange = (value / 10).toFixed(1);
    aimAssistsRangeText.value = fixedAimAssistsRange;
    aimAssistsRange.value = Number(fixedAimAssistsRange);
    aimAssistsRangeSlider.value = value;

    debouncedUpdate();
}

const onChangeAimAssitsConditions = async (conditions: string[]) => {
    aimAssistsConditionsSelected.value = conditions;
    aimAssistsStrafe.value = conditions.includes("strafe_assist"); // Fixed: using underscore to match option value

    if (!aimAssists.value) return;
    await activeAimAssistsModule();
}

const onChangeAimAssists = () => {
    debouncedUpdate();
}

</script>

<template>
    <CardModule module-text="Aim Assist" icon="Handshake" :active-module="aimAssists"
        v-on:module-active="onActiveAimAssists">
        <div class="space-y-6">
            <!-- FOV -->
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>FOV</span>
                    <span class="text-white bg-white/5 px-2 py-0.5 rounded font-mono">{{ aimAssistsFov }}</span>
                </div>
                <Slider :min="20" :max="360" v-model="aimAssistsFov" v-on:change="onChangeAimAssists" />
            </div>

            <!-- Range -->
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Range</span>
                    <span class="text-white bg-white/5 px-2 py-0.5 rounded font-mono">{{ aimAssistsRangeText }}</span>
                </div>
                <Slider :min="30" :max="80" v-model="aimAssistsRangeSlider"
                    v-on:update:model-value="onUpdateAimAssistsRange" />
            </div>

            <!-- Smoothness -->
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Smoothness</span>
                    <span class="text-white bg-white/5 px-2 py-0.5 rounded font-mono">{{ aimAssistsSmooth }}%</span>
                </div>
                <Slider :min="0" :max="100" v-model="aimAssistsSmooth" v-on:change="onChangeAimAssists" />
            </div>

            <!-- Strafe Assist (Conditional) -->
            <transition enter-active-class="transition duration-200 ease-out"
                enter-from-class="transform scale-95 opacity-0" enter-to-class="transform scale-100 opacity-100"
                leave-active-class="transition duration-150 ease-in" leave-from-class="transform scale-100 opacity-100"
                leave-to-class="transform scale-95 opacity-0">
                <div v-if="aimAssistsStrafe" class="p-4 rounded-xl bg-white/5 border border-white/5">
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>Strafe Assist Value</span>
                        <span class="text-indigo-400 font-bold bg-indigo-500/10 px-2 py-0.5 rounded font-mono">{{
                            aimAssistsStrafeText }}</span>
                    </div>
                    <Slider :min="0" :max="50" v-model="aimAssistsStrafeSlider"
                        v-on:update:model-value="onUpdateAimAssistsStrafe" />
                </div>
            </transition>

            <!-- Options -->
            <div class="pt-2 border-t border-white/5">
                <SelectOptions v-model="aimAssistsConditionsSelected" :options="AIM_ASSIST_OPTIONS"
                    :value-black-list="aimAssistsValuesBlackList" v-on:change="onChangeAimAssitsConditions" />
            </div>
        </div>
    </CardModule>
</template>

<style scoped></style>