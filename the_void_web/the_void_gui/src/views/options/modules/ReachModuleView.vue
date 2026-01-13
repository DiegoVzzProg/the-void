<script setup lang="ts">
import { ref, watch } from 'vue';
import CardModule from '../../../components/CardModule.vue';
import SelectOptions, { type SelectOption } from '../../../components/SelectOptions.vue';
import Slider from '../../../components/Slider.vue';
import type { ICommand } from '../../../interfaces/command-module.interface';
import { CommandModuleService } from '../../../services/command-module.service';
import type { IReach } from '../../../interfaces/modules.interface';

const props = defineProps<{ settings: IReach | null }>();

watch(
    () => props.settings,
    (newSettings) => {
        if (newSettings) {
            loadConfig();
        }
    },
    { immediate: true }
);

const loadConfig = () => {
    const settings = props.settings;

    if (!settings)
        return;

    reach.value = settings.action === 'enable' ? true : false;
    reachRange.value = settings.range;
    reachHitChance.value = settings.hitchance;
    reachConditionsSelected.value = settings.conditions;
}

const commandModule = async (commandParams: ICommand) => {
    const commandService = new CommandModuleService();
    const res = await commandService.Command(commandParams);
    const data = res.data;

    if (!data) return;

    if (data.status !== "Connected") {
        return;
    }
}

const reach = ref<boolean>(false);

const reachRange = ref<number>(3.00);
const reachRangeSlider = ref<number>(300);
const reachRangeText = ref<string>("3.00");

const reachHitChance = ref<number>(90);

const reachConditionsSelected = ref<string[]>([]);
const reachOptionsDefault = ref<string[]>([]);
const reachValuesBlackList = ref<string[]>([]);

const reachOptions = ref<SelectOption[]>([
    { label: "randomization", value: "randomization" },
    { label: "sprint only", value: "sprint_only" },
    { label: "hitboxes", value: "hitboxes" },
    { label: "visible only", value: "visible_only" }

]);

const onUpdateReachRange = (value: number) => {
    const fixedReach = (value / 100).toFixed(2);

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

    await commandModule(commandParams);
}

const onChangeReachRange = async () => {
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

</script>

<template>

    <CardModule module-text="reach" icon="Axe" :active-module="reach" v-on:module-active="onActiveReach"
        module-info="Prioritize safety over raw advantage. This module varies your attack distance randomly on each hit using a human probability curve. Objective: Mimic natural imperfection to evade long-term bans on primary accounts.">
        <div class="space-y-5">
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Range</span>
                    <span class="text-white">{{ reachRangeText }}</span>
                </div>
                <Slider :min="300" :max="600" v-model="reachRangeSlider" v-on:update:model-value="onUpdateReachRange"
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
</template>

<style scoped></style>