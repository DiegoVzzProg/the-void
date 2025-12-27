<script setup lang="ts">
import { Axe, Mouse } from 'lucide-vue-next';
import ToogleSwitch from './ToogleSwitch.vue';
const iconMap = {
    Mouse,
    Axe
};

type IconName = keyof typeof iconMap;

const props = defineProps<{
    moduleText: string,
    icon: string
}>();

const getIcon = (name: string) => {
    if (name in iconMap) {
        return iconMap[name as IconName];
    }
    return null;
};
</script>

<template>
    <div
        class="flex flex-col w-full max-md:min-w-full h-full md:h-[240px] bg-white/3 transition-all duration-300 rounded-3xl overflow-hidden border border-[#232732]">
        <div class="flex justify-between items-center gap-3 p-4 bg-black/40">
            <div class="flex items-center gap-3">
                <span class="p-3 rounded-xl bg-black/40 border border-white/10">
                    <component :is="getIcon(props.icon)" class="text-white/40" />
                </span>
                <span v-text="props.moduleText"></span>
            </div>
            <ToogleSwitch />
        </div>

        <div class="flex flex-col flex-1 p-4">
            <slot></slot>
        </div>
    </div>
</template>

<style scoped></style>