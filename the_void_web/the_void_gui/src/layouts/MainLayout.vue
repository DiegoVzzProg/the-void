<script setup lang="ts">
import { onMounted, watch } from 'vue';
import { useRoute } from 'vue-router';
import { storeToRefs } from 'pinia';
import { Form, KeyRound, LayoutDashboard, Package, Settings2, TvMinimal, User } from 'lucide-vue-next';

// Components
import Logo from '../components/Logo.vue';

// Stores & Services
import { useAuthStorage } from '../stores/auth.store';
import { useUserStorage } from '../stores/user.store';
import { Utils } from '../utilities/utilities';

const route = useRoute();
const authStore = useAuthStorage();
const userStore = useUserStorage();
const { optionActive } = storeToRefs(authStore);
const { username, email } = storeToRefs(userStore);

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
    { id: 6, icono: "KeyRound", text: "auth", visible: optionActive.value, params: { view: "login" } },
];

// Determine icon dynamically
const getIcon = (name: string) => {
    if (name && name in iconMap) {
        return iconMap[name as IconName];
    }
    return null;
};

// Navigation handler
const seeOption = (page: string, params: Record<string, string> | undefined) => {
    const site = new Utils();
    site.RedirectPage({
        name: page,
        params: params
    });
};

// Sync auth visibility
watch(
    () => optionActive.value,
    (newValue) => {
        if (options[5]) {
            options[5].visible = newValue ?? false;
        }
    }
);

onMounted(() => {
    // Layout mounted logic if needed
});
</script>

<template>
    <!-- Background Orbs -->
    <div class="fixed top-[-10%] left-[-10%] w-[500px] h-[500px] 
              bg-[#2dd4bf]/10 rounded-full blur-[120px] 
              pointer-events-none animate-pulse z-0">
    </div>

    <div class="fixed bottom-[-10%] right-[-10%] w-[500px] h-[500px] 
              bg-[#f472b6]/10 rounded-full blur-[120px] 
              pointer-events-none animate-pulse z-0" style="animation-delay: 1s;">
    </div>

    <main class="flex flex-col w-full h-dvh overflow-hidden relative z-10 text-slate-300">
        <div class="flex-1 flex lg:items-center lg:justify-center lg:p-4 overflow-hidden">
            <div
                class="w-full max-w-[1200px] bg-[#0a0a0c] h-full lg:max-h-[680px] border border-[#232732] lg:rounded-xl flex max-lg:flex-col-reverse shadow-2xl overflow-hidden">

                <!-- Sidebar -->
                <aside
                    class="flex flex-row lg:flex-col bg-[#0e1012] w-full lg:max-w-[280px] h-full max-lg:max-h-[80px] lg:border-r lg:border-r-[#232732] gap-4 p-4 shrink-0 justify-between">

                    <div class="flex flex-col gap-6 w-full">
                        <div class="flex items-center justify-start px-4 lg:py-2 max-lg:hidden">
                            <Logo />
                        </div>

                        <nav class="flex lg:flex-col max-lg:justify-between gap-2 w-full">
                            <template v-for="item in options" :key="item.id">
                                <button v-if="item.visible" :class="[
                                    'lg:w-full flex items-center p-3 rounded-xl transition-all duration-300 gap-3 group relative overflow-hidden',
                                    route.name === item.text
                                        ? 'bg-indigo-500/10 text-indigo-400 font-medium'
                                        : 'hover:bg-white/5 hover:text-white text-slate-400'
                                ]" @click="seeOption(item.text, item.params as Record<string, string> | undefined)">

                                    <component :is="getIcon(item.icono)"
                                        :class="['transition-all duration-300', route.name === item.text ? 'size-[22px] stroke-[2.5px]' : 'size-[22px] stroke-[1.5px] group-hover:stroke-white']" />

                                    <span class="max-lg:hidden text-sm tracking-wide">{{ item.text }}</span>

                                    <!-- Active indicator strip -->
                                    <div v-if="route.name === item.text"
                                        class="absolute left-0 top-1/2 -translate-y-1/2 w-1 h-8 bg-indigo-500 rounded-r-full shadow-[0_0_10px_#6366f1] max-lg:hidden">
                                    </div>
                                </button>
                            </template>
                        </nav>
                    </div>

                    <!-- User Profile -->
                    <div v-if="username !== ''"
                        class="bg-[#18181B]/50 border border-[#232732] rounded-2xl p-3 lg:p-4 w-full flex flex-row lg:flex-row items-center gap-3 backdrop-blur-md transition-all hover:bg-[#18181B] hover:border-indigo-500/30 group cursor-pointer max-lg:hidden">
                        <div
                            class="size-10 rounded-full overflow-hidden border border-indigo-500/50 bg-[#222] flex items-center justify-center shrink-0 shadow-[0_0_15px_-5px_#6366f1]">
                            <User class="size-6 text-indigo-400" />
                        </div>
                        <div class="flex flex-col overflow-hidden">
                            <span
                                class="text-sm font-semibold text-white truncate group-hover:text-indigo-300 transition-colors">
                                {{ username }}
                            </span>
                            <small class="text-xs text-slate-500 truncate">{{ email }}</small>
                        </div>
                    </div>
                </aside>

                <!-- Main Content Area -->
                <section
                    class="flex flex-col flex-1 relative overflow-hidden bg-gradient-to-br from-[#0a0a0c] to-[#0f1116]">
                    <div
                        class="absolute inset-0 bg-[url('https://grainy-gradients.vercel.app/noise.svg')] opacity-20 pointer-events-none mix-blend-overlay">
                    </div>

                    <div class="flex-1 w-full h-full overflow-y-auto overflow-x-hidden p-1 md:p-4 lg:p-8 scroll-smooth z-10">
                        <slot></slot>
                    </div>
                </section>
            </div>
        </div>
    </main>
</template>

<style scoped>
/* Custom scrollbar for the main content area if needed */
::-webkit-scrollbar {
    width: 6px;
}

::-webkit-scrollbar-track {
    background: transparent;
}

::-webkit-scrollbar-thumb {
    background: #27272a;
    border-radius: 10px;
}

::-webkit-scrollbar-thumb:hover {
    background: #3f3f46;
}
</style>