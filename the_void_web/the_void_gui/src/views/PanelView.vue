<script setup lang="ts">
import { LayoutDashboard, Package, Settings2 } from 'lucide-vue-next';
import { ref } from 'vue';
import DashboardView from './options/DashboardView.vue';
import ModulesView from './options/ModulesView.vue';

const iconMap = {
    LayoutDashboard,
    Package,
    Settings2
};

type IconName = keyof typeof iconMap;
const options = [
    { id: 1, icono: "LayoutDashboard", text: "dashboard" },
    { id: 2, icono: "Package", text: "modules" },
    { id: 3, icono: "Settings2", text: "settings" }
]

const getIcon = (name: string) => {
    if (name in iconMap) {
        return iconMap[name as IconName];
    }
    return null;
};

const selectedOption = ref<number>(2);

const onSelectOption = (id: number): void => {
    selectedOption.value = id;
}

</script>

<template>
    <div class="flex-1 flex md:items-center md:justify-center p-4">
        <div
            class="w-full max-w-[1200px] bg-[#0a0a0c] h-full md:max-h-[580px] border border-[#232732] rounded-4xl flex max-md:flex-col-reverse z-10 overflow-hidden">
            <aside
                class="flex flex-col bg-[#0e1012] w-full md:max-w-3xs h-full max-md:max-h-[80px] nd:border-r md:border-r-[#232732] gap-4 p-4">
                <div class="flex items-center justify-start px-4 md:py-6 max-md:hidden">
                    <div class="flex flex-col items-start leading-[1.1]">
                        <span class="text-[#6366F1] text-[clamp(18px,3vw,24px)] font-semibold">
                            the void
                        </span>
                        <span class="text-[12px] text-[#64748B] google-sans-code">
                            web_client v1.0
                        </span>
                    </div>
                </div>
                <div class="flex md:flex-col max-md:justify-between gap-2 w-full h-full">
                    <button v-for="item in options" :key="item.id" :class="[
                        'md:w-full flex items-center p-3 rounded-xl transition-all duration-200 gap-2',
                        selectedOption == item.id ? 'bg-[#6365f13d] text-[#6366F1]' : 'hover:bg-white/5 hover:text-white text-[#64748B] shadow-inner'
                    ]" v-on:click="onSelectOption(item.id)">
                        <component :is="getIcon(item.icono)"
                            :class="['transition-all duration-200', selectedOption == item.id ? 'size-[28px]' : 'size-[24px]']" />
                        <span class="max-md:hidden">{{ item.text }}</span>
                    </button>
                </div>
            </aside>
            <section class="flex flex-col flex-1 p-4">
                <DashboardView v-if="selectedOption == 1" />
                <ModulesView v-else-if="selectedOption == 2" />
            </section>
        </div>
    </div>
</template>

<style scoped></style>