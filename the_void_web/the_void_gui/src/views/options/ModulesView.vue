<script setup lang="ts">
import { CommandModuleService } from '../../services/command-module.service';
import Slider from '../../components/Slider.vue';
import { ref } from 'vue';
import CardModule from '../../components/CardModule.vue';
import CheckBox from '../../components/CheckBox.vue';

const push = async () => {
    const commandService = new CommandModuleService();

    await commandService.Command("auto-clicker", autoClickerValue);
}

const autoClickerValue = ref<number>(10);

</script>

<template>
    <header class="w-full h-16 flex items-center border-b border-b-[#232732] mb-3">
        <span class="text-[clamp(18px,3vw,24px)]">modules</span>
    </header>

    <div class="flex flex-1 overflow-auto gap-2">
        <CardModule module-text="auto clicker" icon="Mouse">
            <div class="flex flex-col gap-1 flex-1">
                <div class="flex items-center justify-between">
                    <span>cps:</span>
                    <span v-text="autoClickerValue"></span>
                </div>
                <Slider :min="5" :max="24" v-model="autoClickerValue" v-on:change="push()" />
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