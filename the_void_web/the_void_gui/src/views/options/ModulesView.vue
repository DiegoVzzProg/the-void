<script setup lang="ts">
import { CommandModuleService } from '../../services/command-module.service';
import Slider from '../../components/Slider.vue';
import { ref } from 'vue';
import CardModule from '../../components/CardModule.vue';
import SelectOptions, { type SelectOption } from '../../components/SelectOptions.vue';
import type { ICommand } from '../../interfaces/command-module.interface';

const CommandModule = async (commandParams: ICommand) => {
    const commandService = new CommandModuleService();

    const res = await commandService.Command(commandParams);
    const data = res.data;

    if (!data) return;

    if (data.status === "ok") {

    } else {

    }
}

//#region AutoClicker

const autoClicker = ref<boolean>(false);
const autoClickerValue = ref<number>(12);

const autoClickerConditions = ref<string[]>([]);
const autoClickerMaxValue = ref<number>(24);
const autoClickerOptionsDefault = ref<string[]>([]);
const autoClickerValuesBlackList = ref<string[]>([]);

const autoClickerOptions = ref<SelectOption[]>([
    { label: "blatan", value: "blatan" },
    { label: "inventory", value: "inventory" },
    { label: "auto block hit", value: "auto-block-hit" },
    { label: "weapon check", value: "weapon-check" },
    { label: "break blocks", value: "break-blocks" },
    { label: "exhaustion effect", value: "exhaustion" },

]);

const onActiveAutoClicker = async (value: boolean) => {
    autoClicker.value = value;
    await autoClickerModule();
}

const onChangeAutoClickerValue = async () => {
    if (!autoClicker.value) return;

    await autoClickerModule();
}

const onChangeAutoClickerConditions = async (values: string[], valuesOptionsWhiteList: string[]) => {
    if (!autoClicker.value) return;

    if (values.includes("blatan")) {
        autoClickerMaxValue.value = 100;
    } else {
        autoClickerMaxValue.value = 24;
        autoClickerValue.value = 12;
    }

    autoClickerConditions.value = valuesOptionsWhiteList;
    await autoClickerModule();
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

    await CommandModule(commandParams);
}

//#endregion

//#region Reach
const reach = ref<boolean>(false);

const reachRange = ref<number>(3.0);
const reachRangeSlider = ref<number>(30);
const reachRangeText = ref<string>("3.0");

const reachHitChance = ref<number>(50);

const reachConditionsSelected = ref<string[]>([]);
const reachOptionsDefault = ref<string[]>([]);
const reachValuesBlackList = ref<string[]>([]);

const reachOptions = ref<SelectOption[]>([
    { label: "randomization", value: "randomization" },
    { label: "misplace", value: "misplace" },
    { label: "sprint only", value: "sprint-only" },
    { label: "hitboxes", value: "hitboxes" },
    { label: "visible only", value: "visible-only" }

]);

const onUpdateReachRange = (value: number) => {
    const fixedReach = (value / 10).toFixed(1);

    reachRangeText.value = fixedReach;
    reachRange.value = Number(fixedReach);

    reachRangeSlider.value = value;
}

const onActiveReach = async (value: boolean) => {
    reach.value = value;
    await reachModule();
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

    await CommandModule(commandParams);
}

const onChangeReachRange = async (value: number) => {
    const fixedReach = (value / 10).toFixed(1);
    reachRange.value = Number(fixedReach);

    if (!reach.value) return;

    await reachModule();
}

const onChangeHitChance = async (value: number) => {
    reachHitChance.value = value;

    if (!reach.value) return;
    await reachModule();
}

const onChangeConditionsReachRange = async (conditions: string[]) => {
    reachConditionsSelected.value = conditions;

    if (!reach.value) return;
    await reachModule();
}

//#endregion

//#region AimAssists
const aimAssists = ref<boolean>(false);
const aimAssistsFov = ref<number>(40);

const aimAssistsRange = ref<number>(3.0);
const aimAssistsRangeSlider = ref<number>(40);
const aimAssistsRangeText = ref<string>("4.0");

const aimAssistsSmooth = ref<number>(15);

const aimAssistsConditionsSelected = ref<string[]>([]);
const aimAssistsOptionsDefault = ref<string[]>([]);
const aimAssistsValuesBlackList = ref<string[]>([]);

const aimAssistsOptions = ref<SelectOption[]>([
    { label: "weapon check", value: "weapon-check" },
    { label: "break blocks", value: "break-blocks" },
    { label: "click only", value: "click-only" }

]);

const onUpdateAimAssistsRange = (value: number) => {
    const fixedAimAssistsRange = (value / 10).toFixed(1);

    aimAssistsRangeText.value = fixedAimAssistsRange;
    aimAssistsRange.value = Number(fixedAimAssistsRange);

    aimAssistsRangeSlider.value = value;
}

const onActiveAimAssists = async (value: boolean) => {
    aimAssists.value = value;
    await activeAimAssistsModule();
}

const onChangeAimAssitsConditions = async (conditions: string[]) => {
    aimAssistsConditionsSelected.value = conditions;

    if (!aimAssists.value) return;
    await activeAimAssistsModule();
}

const onChangeAimAssistsRange = async () => {
    if (!aimAssists.value) return;

    await activeAimAssistsModule();
}

const onChangeAimAssistsFov = async () => {
    if (!aimAssists.value) return;

    await activeAimAssistsModule();
}

const onChangeAimAssistsSmoothness = async () => {
    if (!aimAssists.value) return;

    await activeAimAssistsModule();
}

const activeAimAssistsModule = async () => {
    const commandParams: ICommand = {
        module: "aim-assists",
        action: aimAssists.value ? "enable" : "disable",
        settings: {
            fov: aimAssistsFov.value,
            smoothness: aimAssistsSmooth.value,
            range: aimAssistsRange.value,
        },
        conditions: aimAssistsConditionsSelected.value
    }
    await CommandModule(commandParams);
}

//#endregion

//#region Velocity
const velocity = ref<boolean>(false);

const velocityHorizontal = ref<number>(100);
const velocityVertical = ref<number>(100);
const velocityChance = ref<number>(100);

const velocityConditionsSelected = ref<string[]>([]);
const velocityOptionsDefault = ref<string[]>([]);
const velocityValuesBlackList = ref<string[]>([]);

const velocityOptions = ref<SelectOption[]>([
    { label: "w-tap only", value: "w-tap-only" },
    { label: "targeting", value: "targeting" }
]);

const onChangeVelocityConditions = async (conditions: string[]) => {
    velocityConditionsSelected.value = conditions;

    if (!velocity.value) return;
    await velocityModule();
}

const onActiveVelocity = async (value: boolean) => {
    velocity.value = value;
    await velocityModule();
}

const onChangeVelocityHorizontal = async () => {
    if (!velocity.value) return;

    await velocityModule();
}

const onChangeVelocityVertical = async () => {
    if (!velocity.value) return;

    await velocityModule();
}

const onChangeVelocityChance = async () => {
    if (!velocity.value) return;

    await velocityModule();
}

const velocityModule = async () => {
    const commandParams: ICommand = {
        module: "velocity",
        action: velocity.value ? "enable" : "disable",
        settings: {
            horizontal: velocityHorizontal.value,
            vertical: velocityVertical.value,
            chance: velocityChance.value
        },
        conditions: velocityConditionsSelected.value
    }
    await CommandModule(commandParams);
}

//#endregion

</script>

<template>
    <header class="w-full min-h-16 flex items-center mb-3">
        <h1 class="text-2xl font-bold text-white tracking-tight flex items-center gap-2">
            <span class="w-2 h-6 bg-[#6366f1] rounded-full"></span>
            modules
        </h1>
    </header>

    <div class="grid grid-cols-1 md:grid-cols-2 gap-4 lg:gap-6 pb-10 overflow-auto h-[calc(100dvh-186px)]">

        <CardModule module-text="auto clicker" icon="Mouse" v-on:module-active="onActiveAutoClicker">
            <div class="space-y-6">
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>CPS</span>
                        <span class="text-white">{{ autoClickerValue }}</span>
                    </div>
                    <Slider :min="5" :max="autoClickerMaxValue" v-model="autoClickerValue"
                        v-on:change="onChangeAutoClickerValue" />
                </div>
                <SelectOptions :model-value="autoClickerOptionsDefault" :options="autoClickerOptions"
                    :value-black-list="autoClickerValuesBlackList" v-on:change="onChangeAutoClickerConditions" />
            </div>
        </CardModule>

        <CardModule module-text="reach" icon="Axe" v-on:module-active="onActiveReach">
            <div class="space-y-5">
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>Range</span>
                        <span class="text-white">{{ reachRangeText }}</span>
                    </div>
                    <Slider :min="30" :max="80" v-model="reachRangeSlider" v-on:update:model-value="onUpdateReachRange"
                        v-on:change="onChangeReachRange" />
                </div>
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>Hit Chance</span>
                        <span class="text-white">{{ reachHitChance }}%</span>
                    </div>
                    <Slider :min="0" :max="100" v-model="reachHitChance" v-on:change="onChangeHitChance" />
                </div>
                <SelectOptions v-model="reachOptionsDefault" :options="reachOptions"
                    :value-black-list="reachValuesBlackList" v-on:change="onChangeConditionsReachRange" />
            </div>
        </CardModule>

        <CardModule module-text="aim assist" icon="Handshake" v-on:module-active="onActiveAimAssists">
            <div class="space-y-5">
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>FOV</span>
                        <span class="text-white">{{ aimAssistsFov }}</span>
                    </div>
                    <Slider :min="20" :max="360" v-model="aimAssistsFov" v-on:change="onChangeAimAssistsFov" />
                </div>
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>Range</span>
                        <span class="text-white">{{ aimAssistsRangeText }}</span>
                    </div>
                    <Slider :min="30" :max="80" v-model="aimAssistsRangeSlider"
                        v-on:update:model-value="onUpdateAimAssistsRange" v-on:change="onChangeAimAssistsRange" />
                </div>
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>Smoothness</span>
                        <span class="text-white">{{ aimAssistsSmooth }}%</span>
                    </div>
                    <Slider :min="0" :max="100" v-model="aimAssistsSmooth" v-on:change="onChangeAimAssistsSmoothness" />
                </div>
                <SelectOptions v-model="aimAssistsOptionsDefault" :options="aimAssistsOptions"
                    :value-black-list="aimAssistsValuesBlackList" v-on:change="onChangeAimAssitsConditions" />
            </div>
        </CardModule>

        <CardModule module-text="velocity" icon="UnfoldHorizontal" v-on:module-active="onActiveVelocity">
            <div class="space-y-5">
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>Horizontal</span>
                        <span class="text-white">{{ velocityHorizontal }}%</span>
                    </div>
                    <Slider :min="0" :max="100" v-model="velocityHorizontal" v-on:change="onChangeVelocityHorizontal" />
                </div>
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>Vertical</span>
                        <span class="text-white">{{ velocityVertical }}%</span>
                    </div>
                    <Slider :min="0" :max="100" v-model="velocityVertical" v-on:change="onChangeVelocityVertical" />
                </div>
                <div>
                    <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                        <span>Chance</span>
                        <span class="text-white">{{ velocityChance }}%</span>
                    </div>
                    <Slider :min="0" :max="100" v-model="velocityChance" v-on:change="onChangeVelocityChance" />
                </div>
                <SelectOptions v-model="velocityOptionsDefault" :options="velocityOptions"
                    :value-black-list="velocityValuesBlackList" v-on:change="onChangeVelocityConditions" />
            </div>
        </CardModule>
    </div>
</template>

<style scoped></style>