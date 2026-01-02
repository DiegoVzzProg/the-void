<script setup lang="ts">
import { ref } from 'vue';
import CardModule from '../../../components/CardModule.vue';
import SelectOptions, { type SelectOption } from '../../../components/SelectOptions.vue';
import Slider from '../../../components/Slider.vue';
import type { ICommand } from '../../../interfaces/command-module.interface';
import { CommandModuleService } from '../../../services/command-module.service';


const commandModule = async (commandParams: ICommand) => {
    const commandService = new CommandModuleService();

    const res = await commandService.Command(commandParams);
    const data = res.data;

    if (!data) return;

    if (data.status === "ok") {

    } else {

    }
}

const reach = ref<boolean>(false);

const reachRange = ref<number>(3.00);
const reachRangeSlider = ref<number>(300);
const reachRangeText = ref<string>("3.00");

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
    const fixedReach = (value / 100).toFixed(2);

    reachRangeText.value = fixedReach;
    reachRange.value = Number(fixedReach);

    console.log(Number(fixedReach), fixedReach, reachRange.value);
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

    <CardModule module-text="reach" icon="Axe" v-on:module-active="onActiveReach">
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