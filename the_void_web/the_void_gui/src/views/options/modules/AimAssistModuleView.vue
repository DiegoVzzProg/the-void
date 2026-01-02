<script setup lang="ts">
import { ref } from 'vue';
import CardModule from '../../../components/CardModule.vue';
import SelectOptions, { type SelectOption } from '../../../components/SelectOptions.vue';
import Slider from '../../../components/Slider.vue';
import { CommandModuleService } from '../../../services/command-module.service';
import type { ICommand } from '../../../interfaces/command-module.interface';

const commandModule = async (commandParams: ICommand) => {
    const commandService = new CommandModuleService();

    const res = await commandService.Command(commandParams);
    const data = res.data;

    if (!data) return;

    if (data.status === "ok") {

    } else {

    }
}

const aimAssists = ref<boolean>(false);
const aimAssistsFov = ref<number>(35);

const aimAssistsRange = ref<number>(4.0);
const aimAssistsRangeSlider = ref<number>(40);
const aimAssistsRangeText = ref<string>("4.0");

const aimAssistsSmooth = ref<number>(18);

const aimAssistsConditionsSelected = ref<string[]>([]);
const aimAssistsOptionsDefault = ref<string[]>([]);
const aimAssistsValuesBlackList = ref<string[]>([]);


const aimAssistsStrafe = ref<boolean>(false);

const aimAssistsStrafeValue = ref<number>(3);
const aimAssistsStrafeSlider = ref<number>(40);
const aimAssistsStrafeText = ref<string>("4.0");

const aimAssistsOptions = ref<SelectOption[]>([
    { label: "weapon check", value: "weapon-check" },
    { label: "break blocks", value: "break-blocks" },
    { label: "click only", value: "click-only" },
    { label: "visibility check", value: "visibility-check" },
    { label: "strafe assist", value: "strafe-assist" },

]);

const onUpdateAimAssistsStrafe = (value: number) => {
    const fixedAimAssistsStrafe = (value / 10).toFixed(1);

    aimAssistsStrafeText.value = fixedAimAssistsStrafe;
    aimAssistsStrafeValue.value = Number(fixedAimAssistsStrafe);

    aimAssistsStrafeSlider.value = value;
}

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
    aimAssistsStrafe.value = conditions.includes("strafe-assist");


    if (!aimAssists.value) return;
    await activeAimAssistsModule();
}

const onChangeAimAssists = async () => {
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
            strafe_assist_value: aimAssistsStrafeValue.value
        },
        conditions: aimAssistsConditionsSelected.value
    }
    await commandModule(commandParams);
}

</script>

<template>
    <CardModule module-text="aim assist" icon="Handshake" v-on:module-active="onActiveAimAssists">
        <div class="space-y-5">
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>FOV</span>
                    <span class="text-white">{{ aimAssistsFov }}</span>
                </div>
                <Slider :min="20" :max="360" v-model="aimAssistsFov" v-on:change="onChangeAimAssists" />
            </div>
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Range</span>
                    <span class="text-white">{{ aimAssistsRangeText }}</span>
                </div>
                <Slider :min="30" :max="80" v-model="aimAssistsRangeSlider"
                    v-on:update:model-value="onUpdateAimAssistsRange" v-on:change="onChangeAimAssists" />
            </div>
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Smoothness</span>
                    <span class="text-white">{{ aimAssistsSmooth }}%</span>
                </div>
                <Slider :min="0" :max="100" v-model="aimAssistsSmooth" v-on:change="onChangeAimAssists" />
            </div>
            <div v-if="aimAssistsStrafe">
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Strafe Assist</span>
                    <span class="text-white">{{ aimAssistsStrafeText }}</span>
                </div>
                <Slider :min="0" :max="50" v-model="aimAssistsStrafeSlider"
                    v-on:update:model-value="onUpdateAimAssistsStrafe" v-on:change="onChangeAimAssists" />
            </div>
            <SelectOptions v-model="aimAssistsOptionsDefault" :options="aimAssistsOptions"
                :value-black-list="aimAssistsValuesBlackList" v-on:change="onChangeAimAssitsConditions" />
        </div>
    </CardModule>
</template>

<style scoped></style>