<script setup lang="ts">
import { Axe, Handshake, Mouse, UnfoldHorizontal } from 'lucide-vue-next';
import ToogleSwitch from './ToogleSwitch.vue';
import { ref } from 'vue';

// ... (Tu lógica de iconos se mantiene igual) ...
type IconName = keyof typeof iconMap;
const iconMap = { Mouse, Axe, Handshake, UnfoldHorizontal };

const emits = defineEmits<{ (e: 'moduleActive', value: boolean): void; }>();
const props = defineProps<{ moduleText: string, icon: string }>();

const getIcon = (name: string) => iconMap[name as IconName] || null;

const checkedModule = ref<boolean>(false);

const onModuleActive = () => {
    emits("moduleActive", checkedModule.value);
}
</script>

<template>
    <div class="group relative flex flex-col w-full min-h-[420px] rounded-2xl border transition-all duration-500 overflow-hidden"
        :class="[
            checkedModule
                ? 'bg-[#0c0c12]/80 border-[#6366f1]/50 shadow-[0_0_30px_-10px_rgba(99,102,241,0.15)]'
                : 'bg-[#0c0c12]/60 border-[#232732] hover:border-white/20'
        ]">
        <div class="flex justify-between items-center p-5 border-b transition-colors duration-300"
            :class="checkedModule ? 'border-[#6366f1]/20 bg-[#6366f1]/5' : 'border-white/5 bg-transparent'">

            <div class="flex items-center gap-4">
                <div class="p-2.5 rounded-lg border transition-all duration-300"
                    :class="checkedModule ? 'bg-[#6366f1] border-[#6366f1] text-white shadow-[0_0_15px_rgba(99,102,241,0.5)]' : 'bg-[#18181b] border-white/10 text-zinc-500'">
                    <component :is="getIcon(props.icon)" :size="20" />
                </div>
                <span class="font-medium tracking-wide text-sm uppercase"
                    :class="checkedModule ? 'text-white drop-shadow-sm' : 'text-zinc-400'">
                    {{ props.moduleText }}
                </span>
            </div>

            <ToogleSwitch v-model="checkedModule" v-on:change="onModuleActive" />
        </div>

        <div class="flex flex-col flex-1 p-5 gap-5 overflow-y-auto custom-scrollbar">
            <slot></slot>
        </div>
    </div>
</template>

<style scoped>
/* Scrollbar fino para que no estorbe */
.custom-scrollbar::-webkit-scrollbar {
    width: 4px;
}

.custom-scrollbar::-webkit-scrollbar-track {
    background: transparent;
}

.custom-scrollbar::-webkit-scrollbar-thumb {
    background: #3f3f46;
    border-radius: 4px;
}

.custom-scrollbar::-webkit-scrollbar-thumb:hover {
    background: #52525b;
}
</style>