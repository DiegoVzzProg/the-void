<script setup lang="ts">
import { Form, KeyRound, LayoutDashboard, Package, Settings2, TvMinimal, User } from 'lucide-vue-next';
import { onMounted, watch } from 'vue';
import Logo from '../components/Logo.vue';
import { useAuthStorage } from '../stores/auth.store';
import { useUserStorage } from '../stores/user.store';
import { Utils } from '../utilities/utilities';
import { useRoute } from 'vue-router';


const router = useRoute();
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
    { id: 1, icono: "TvMinimal", text: "information", visible: true, params: {} },
    { id: 2, icono: "LayoutDashboard", text: "dashboard", visible: false, params: {} },
    { id: 3, icono: "Package", text: "modules", visible: true, params: {} },
    { id: 4, icono: "Settings2", text: "settings", visible: false, params: {} },
    { id: 5, icono: "Form", text: "forum", visible: false, params: {} },
    { id: 6, icono: "KeyRound", text: "auth", visible: useAuthStorage().optionActive, params: { view: "login" } },
]

onMounted(() => {
})

watch(
    () => useAuthStorage().optionActive,
    (newValue) => {
        console.log(newValue);
        if (options[5]) {
            console.log(newValue);

            options[5].visible = newValue ?? false;
        }
    }
);

const getIcon = (name: string) => {
    if (name && name in iconMap) {
        return iconMap[name as IconName];
    }
    return null;
};

const seeOption = (page: string, params: Record<string, string> | undefined) => {
    const site = new Utils();
    site.RedirectPage({
        name: page,
        params: params
    });
}

</script>

<template>
    <!-- Orbe 1: Brillo Teal (Arriba Izquierda) -->
    <div class="absolute top-[-10%] left-[-10%] w-[500px] h-[500px] 
              bg-[#2dd4bf]/10 rounded-full blur-[120px] 
              pointer-events-none animate-pulse">
    </div>

    <!-- Orbe 2: Brillo Rosa (Abajo Derecha con Retraso) -->
    <div class="absolute bottom-[-10%] right-[-10%] w-[500px] h-[500px] 
              bg-[#f472b6]/10 rounded-full blur-[120px] 
              pointer-events-none animate-pulse" style="animation-delay: 1s;">
    </div>

    <main class="flex flex-col w-full h-dvh overflow-auto">
        <div class="flex-1 flex lg:items-center lg:justify-center lg:p-4">
            <div
                class="w-full max-w-[1200px] bg-[#0a0a0c] h-full lg:max-h-[680px] border border-[#232732] lg:rounded-4xl flex max-lg:flex-col-reverse z-10 overflow-hidden">
                <aside
                    class="flex flex-row lg:flex-col bg-[#0e1012] w-full lg:max-w-3xs h-full max-lg:max-h-[80px] lg:border-r lg:border-r-[#232732] gap-4 p-4">
                    <div class="flex items-center justify-start px-4 lg:py-6 max-lg:hidden">
                        <Logo />
                    </div>
                    <div class="flex lg:flex-col max-lg:justify-between gap-2 flex-1">
                        <template v-for="item in options" :key="item.id">
                            <button v-if="item.visible" :class="[
                                'lg:w-full flex items-center p-3 rounded-xl transition-all duration-200 gap-2',
                                router.name === item.text ? 'bg-[#6365f13d] text-[#6366F1]' : 'hover:bg-white/5 hover:text-white text-[#64748B] shadow-inner'
                            ]" @click="seeOption(item.text, item.params as Record<string, string> | undefined)">
                                <component :is="getIcon(item.icono)"
                                    :class="['transition-all duration-200', router.name === item.text ? 'size-[28px]' : 'size-[24px]']" />
                                <span class="max-lg:hidden">{{ item.text }}</span>
                            </button>
                        </template>
                    </div>
                    <div v-if="useUserStorage().username !== ''"
                        class="bg-[#18181B] border border-[#232732] rounded-xl lg:py-4 px-4 lg:w-full flex flex-row lg:flex-col items-center gap-2 lg:mb-2 shadow-lg">
                        <button
                            class="lg:size-16 rounded-full overflow-hidden border border-indigo-500 bg-[#222] flex items-center justify-center">
                            <User class="p-1 lg:size-8 stroke-slate-400" />
                        </button>
                        <div class="flex flex-col items-center text-sm lg:text-md">
                            <span class="gradient-text lg:text-white leading-tight" v-text="useUserStorage().username">
                            </span>
                            <small class="text-[#64748B]" v-text="useUserStorage().email"></small>
                        </div>
                    </div>
                </aside>
                <section class="flex flex-col flex-1 p-2 lg:p-4">
                    <slot></slot>
                </section>
            </div>
        </div>
    </main>
</template>

<style scoped></style>