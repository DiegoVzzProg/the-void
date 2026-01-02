<script setup lang="ts">
import { ref } from 'vue';
import type { ICommand } from '../../../interfaces/command-module.interface';
import type { SelectOption } from '../../../components/SelectOptions.vue';
import { CommandModuleService } from '../../../services/command-module.service';
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

const onChangeVelocity = async () => {
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
    await commandModule(commandParams);
}

</script>

<template>
    <CardModule module-text="velocity" icon="UnfoldHorizontal" v-on:module-active="onActiveVelocity">
        <div class="space-y-5">
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Horizontal</span>
                    <span class="text-white">{{ velocityHorizontal }}%</span>
                </div>
                <Slider :min="0" :max="100" v-model="velocityHorizontal" v-on:change="onChangeVelocity" />
            </div>
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Vertical</span>
                    <span class="text-white">{{ velocityVertical }}%</span>
                </div>
                <Slider :min="0" :max="100" v-model="velocityVertical" v-on:change="onChangeVelocity" />
            </div>
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Chance</span>
                    <span class="text-white">{{ velocityChance }}%</span>
                </div>
                <Slider :min="0" :max="100" v-model="velocityChance" v-on:change="onChangeVelocity" />
            </div>
            <SelectOptions v-model="velocityOptionsDefault" :options="velocityOptions"
                :value-black-list="velocityValuesBlackList" v-on:change="onChangeVelocityConditions" />
        </div>
    </CardModule>
</template>

<style scoped></style>