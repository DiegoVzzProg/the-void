<script setup lang="ts">
import { Form, KeyRound, LayoutDashboard, Package, Settings2, TvMinimal } from 'lucide-vue-next';
import { ref } from 'vue';
import DashboardView from './options/DashboardView.vue';
import ModulesView from './options/ModulesView.vue';
import Logo from '../components/Logo.vue';
import WelcomeView from './options/WelcomeView.vue';
import AuthView from './AuthView.vue';

const iconMap = {
    LayoutDashboard,
    Package,
    Settings2,
    TvMinimal,
    Form,
    KeyRound
};

type IconName = keyof typeof iconMap;
const options = [
    { id: 1, icono: "TvMinimal", text: "information", visible: true },
    { id: 2, icono: "LayoutDashboard", text: "dashboard", visible: false },
    { id: 3, icono: "Package", text: "modules", visible: true },
    { id: 4, icono: "Settings2", text: "settings", visible: false },
    { id: 5, icono: "Form", text: "forum", visible: false },
    { id: 6, icono: "KeyRound", text: "auth", visible: true },
]

const getIcon = (name: string) => {
    if (name in iconMap) {
        return iconMap[name as IconName];
    }
    return null;
};

const selectedOption = ref<number>(1);

const onSelectOption = (id: number): void => {
    selectedOption.value = id;
}

</script>

<template>
    <div class="flex-1 flex lg:items-center lg:justify-center lg:p-4">
        <div
            class="w-full max-w-[1200px] bg-[#0a0a0c] h-full lg:max-h-[680px] border border-[#232732] lg:rounded-4xl flex max-lg:flex-col-reverse z-10 overflow-hidden">
            <aside
                class="flex flex-col bg-[#0e1012] w-full lg:max-w-3xs h-full max-lg:max-h-[80px] lg:border-r lg:border-r-[#232732] gap-4 p-4">
                <div class="flex items-center justify-start px-4 lg:py-6 max-lg:hidden">
                    <Logo />
                </div>
                <div class="flex lg:flex-col max-lg:justify-between gap-2 w-full h-full">
                    <template v-for="item in options" :key="item.id">
                        <button v-if="item.visible" :class="[
                            'lg:w-full flex items-center p-3 rounded-xl transition-all duration-200 gap-2',
                            selectedOption == item.id ? 'bg-[#6365f13d] text-[#6366F1]' : 'hover:bg-white/5 hover:text-white text-[#64748B] shadow-inner'
                        ]" v-on:click="onSelectOption(item.id)">
                            <component :is="getIcon(item.icono)"
                                :class="['transition-all duration-200', selectedOption == item.id ? 'size-[28px]' : 'size-[24px]']" />
                            <span class="max-lg:hidden">{{ item.text }}</span>
                        </button>
                    </template>
                </div>
            </aside>
            <section class="flex flex-col flex-1 p-2 lg:p-4">
                <WelcomeView v-if="selectedOption == 1" />
                <DashboardView v-else-if="selectedOption == 2" />
                <ModulesView v-else-if="selectedOption == 3" />
                <AuthView v-else-if="selectedOption == 6" />
            </section>
        </div>
    </div>
</template>

<style scoped></style>