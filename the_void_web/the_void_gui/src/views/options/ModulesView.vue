<script setup lang="ts">
import { CommandModuleService } from '../../services/command-module.service';
import Slider from '../../components/Slider.vue';
import { ref } from 'vue';
import CardModule from '../../components/CardModule.vue';
import CheckBox from '../../components/CheckBox.vue';

const autoClicker = ref<boolean>(false);
const autoClickerValue = ref<number>(12);

const auto_clicker_active = async (value: boolean) => {
    autoClicker.value = value;
    await autoClickerModule(autoClicker.value, autoClickerValue.value);
}

const auto_clicker_value = async () => {
    await autoClickerModule(autoClicker.value, autoClickerValue.value);
}

const autoClickerModule = async (enabled: boolean, value: number) => {
    const commandService = new CommandModuleService();
    await commandService.Command("auto-clicker", enabled, value);
}



</script>

<template>
    <header class="w-full h-16 flex items-center border-b border-b-[#232732] mb-3">
        <span class="text-[clamp(18px,3vw,24px)]">modules</span>
    </header>

    <div class="flex flex-1 overflow-auto gap-2">
        <CardModule module-text="auto clicker" icon="Mouse" v-on:module-active="auto_clicker_active">
            <div class="flex flex-col gap-1 flex-1">
                <div class="flex items-center justify-between">
                    <span>cps:</span>
                    <span v-text="autoClickerValue"></span>
                </div>
                <Slider :min="5" :max="24" v-model="autoClickerValue" v-on:change="auto_clicker_value" />
            </div>
            <div class="flex flex-col flex-1">
                <div class="flex w-full items-center justify-between">
                    <span>auto block hit</span>
                    <CheckBox />
                </div>
            </div>
        </CardModule>
        <CardModule module-text="reach" icon="Axe"></CardModule>
    </div>
</template>

<style scoped></style>