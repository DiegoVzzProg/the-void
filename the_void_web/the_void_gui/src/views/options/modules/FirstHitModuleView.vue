<script setup lang="ts">
import { ref, watch } from 'vue';
import CardModule from '../../../components/CardModule.vue';
import Slider from '../../../components/Slider.vue';
import type { ICommand } from '../../../interfaces/command-module.interface';
import { CommandModuleService } from '../../../services/command-module.service';
import type { IFirstHit } from '../../../interfaces/modules.interface';

const props = defineProps<{ settings: IFirstHit | null }>();

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

    hit.value = settings.action === 'enable' ? true : false;
    hitRange.value = settings.range;
}

const hit = ref<boolean>(false);
const hitRangeText = ref<string>("0.00");
const hitRange = ref<number>(0.00);
const hitSlider = ref<number>(0);

const onUpdateHitRange = (value: number) => {
    const fixedReach = (value / 100).toFixed(2);

    hitRangeText.value = fixedReach;
    hitRange.value = Number(fixedReach);

    hitSlider.value = value;
}

const onActiveHit = async (value: boolean) => {
    hit.value = value;

    if (!hit.value) return;

    await onChangeHitRange();
}

const onChangeHitRange = async () => {
    if (!hit.value) return;

    const commandService = new CommandModuleService();
    const commandParams: ICommand = {
        module: "first-hit",
        action: hit.value ? "enable" : "disable",
        settings: {
            range: hitRange.value
        }
    }

    const res = await commandService.Command(commandParams);
    const data = res.data;

    if (!data) return;

    if (data.status !== "Connected") {
        return;
    }
}

</script>

<template>
    <CardModule module-text="first hit (combo)" icon="HandFist" :active-module="hit" v-on:module-active="onActiveHit"
        module-info="Prioritize competitive advantage. Maintains a consistent extended distance camouflaged under a simulation of network latency. Objective: Mathematically guarantee that your blow connects before the opponent's in each exchange, simulating lag.">
        <div class="space-y-5">
            <div>
                <div class="flex justify-between text-xs font-semibold text-zinc-400 mb-2 uppercase tracking-wider">
                    <span>Range</span>
                    <span class="text-white" v-text="hitRangeText"></span>
                </div>
                <Slider :min="0" :max="50" v-model="hitSlider" v-on:update:model-value="onUpdateHitRange"
                    v-on:change="onChangeHitRange" />
            </div>
        </div>
    </CardModule>
</template>