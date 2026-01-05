<script setup lang="ts">
import { ref } from 'vue';
import { CommandModuleService } from '../../../services/command-module.service';
import type { ICommand } from '../../../interfaces/command-module.interface';
import type { SelectOption } from '../../../components/SelectOptions.vue';
import CardModule from '../../../components/CardModule.vue';
import Slider from '../../../components/Slider.vue';
import SelectOptions from '../../../components/SelectOptions.vue';

const commandModule = async (commandParams: ICommand) => {
    const commandService = new CommandModuleService();

    const res = await commandService.Command(commandParams);
    const data = res.data;

    if (!data) return;

    if (data.status === "ok") {

    } else {

    }
}

const autoClicker = ref<boolean>(false);
const autoClickerValue = ref<number>(12);

const autoClickerConditions = ref<string[]>([]);

const autoClickerSliderMin = ref<number>(5);
const autoClickerSliderMax = ref<number>(24);

const autoClickerOptionsDefault = ref<string[]>([]);
const autoClickerValuesBlackList = ref<string[]>([]);

const autoClickerOptions = ref<SelectOption[]>([
    { label: "blatan", value: "blatan" },
    { label: "inventory", value: "inventory" },
    { label: "auto block hit", value: "auto-block-hit" },
    { label: "weapon check", value: "weapon-check" },
    { label: "ignore blocks", value: "ignore-blocks" }

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
        autoClickerSliderMax.value = 80;
    } else if (!values.includes("blatan") && autoClickerSliderMax.value === 80) {
        autoClickerSliderMax.value = 24;
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

    await commandModule(commandParams);
}

</script>

<template>
    <CardModule module-text="auto clicker" icon="Mouse" v-on:module-active="onActiveAutoClicker">
        <div class="space-y-6">
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>CPS</span>
                    <span class="text-white">{{ autoClickerValue }}</span>
                </div>
                <Slider :min="autoClickerSliderMin" :max="autoClickerSliderMax" v-model="autoClickerValue"
                    v-on:change="onChangeAutoClickerValue" />
            </div>
            <SelectOptions :model-value="autoClickerOptionsDefault" :options="autoClickerOptions"
                :value-black-list="autoClickerValuesBlackList" v-on:change="onChangeAutoClickerConditions" />
        </div>
    </CardModule>
</template>

<style scoped></style>