<script setup lang="ts">
import Swal from 'sweetalert2'

import { Axe, CircleQuestionMark, HandFist, Handshake, Mouse, UnfoldHorizontal } from 'lucide-vue-next';
import ToogleSwitch from './ToogleSwitch.vue';
import { ref, watch } from 'vue';

// ... (Tu lógica de iconos se mantiene igual) ...
type IconName = keyof typeof iconMap;
const iconMap = { Mouse, Axe, Handshake, UnfoldHorizontal, HandFist };

const emits = defineEmits<{ (e: 'moduleActive', value: boolean): void; }>();
const props = defineProps<{ moduleText: string, icon: string, moduleInfo?: string, activeModule: boolean }>();

const getIcon = (name: string) => iconMap[name as IconName] || null;

const checkedModule = ref<boolean>(false);

const onModuleActive = () => {
    emits("moduleActive", checkedModule.value);
}

watch(
    () => props.activeModule,
    (value) => {
        checkedModule.value = value;
        if (value)
            emits("moduleActive", checkedModule.value);
    },
    { immediate: true }
);

const ToolTip = () => {
    if (!props.moduleInfo) return;

    Swal.fire({
        title: 'info',
        text: props.moduleInfo,
        icon: 'question',
        confirmButtonText: 'Ok',
        background: '#13151b',
        color: '#ffffff',
        backdrop: `rgba(0,0,0,0.8)`,
        buttonsStyling: false,
        customClass: {
            popup: 'void-modal',
            confirmButton: 'void-button',
            title: 'void-title',
            htmlContainer: 'void-text',
            icon: 'void-icon'
        }
    })
}

</script>

<template>
    <div class="group relative flex flex-col w-full min-h-[420px] max-h-[420px] rounded-2xl border transition-all duration-500 overflow-hidden"
        :class="[
            checkedModule
                ? 'bg-[#0c0c12]/80 chroma-border chroma-glow'
                : 'bg-[#0c0c12]/60 border-[#232732] hover:border-white/20'
        ]">
        <div class="flex justify-between items-center p-5 border-b transition-colors duration-300"
            :class="checkedModule ? 'border-indigo-500/20 bg-indigo-500/5' : 'border-white/5 bg-transparent'">

            <div class="flex items-center gap-4">
                <div class="p-2.5 rounded-lg border transition-all duration-300"
                    :class="checkedModule ? 'chroma-bg border-transparent text-white shadow-lg' : 'bg-[#18181b] border-white/10 text-zinc-500'">
                    <component :is="getIcon(props.icon)" :size="20" />
                </div>
                <span class="font-medium tracking-wide text-sm uppercase"
                    :class="checkedModule ? 'text-white drop-shadow-sm' : 'text-zinc-400'">
                    {{ props.moduleText }}
                </span>
                <button v-if="props.moduleInfo" v-on:click="ToolTip">
                    <CircleQuestionMark class="size-4"
                        :class="checkedModule ? 'stroke-indigo-400 chroma-text shadow-[0_0_15px_rgba(99,102,241,0.5)] rounded-full' : 'stroke-zinc-500'" />
                </button>
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

/* --- CHROMA EFFECT (Blue-Purple Only) --- */
@keyframes void-chroma {
    0% {
        border-color: rgba(99, 102, 241, 0.5);
        /* Indigo-500 */
        box-shadow: 0 0 30px -10px rgba(99, 102, 241, 0.15);
    }

    33% {
        border-color: rgba(139, 92, 246, 0.5);
        /* Violet-500 */
        box-shadow: 0 0 30px -10px rgba(139, 92, 246, 0.15);
    }

    66% {
        border-color: rgba(59, 130, 246, 0.5);
        /* Blue-500 */
        box-shadow: 0 0 30px -10px rgba(59, 130, 246, 0.15);
    }

    100% {
        border-color: rgba(99, 102, 241, 0.5);
        /* Indigo-500 */
        box-shadow: 0 0 30px -10px rgba(99, 102, 241, 0.15);
    }
}

@keyframes void-chroma-bg {
    0% {
        background-color: #6366f1;
    }

    /* Indigo */
    33% {
        background-color: #8b5cf6;
    }

    /* Violet */
    66% {
        background-color: #3b82f6;
    }

    /* Blue */
    100% {
        background-color: #6366f1;
    }
}

@keyframes void-chroma-text {
    0% {
        stroke: #6366f1;
    }

    33% {
        stroke: #8b5cf6;
    }

    66% {
        stroke: #3b82f6;
    }

    100% {
        stroke: #6366f1;
    }
}

.chroma-border {
    animation: void-chroma 6s infinite linear;
    /* Smooth 6s cycle */
    border-width: 1px;
    border-style: solid;
}

.chroma-bg {
    animation: void-chroma-bg 6s infinite linear;
}

.chroma-text {
    animation: void-chroma-text 6s infinite linear;
}


/* --- Estilos del Modal (La tarjeta oscura) --- */
.void-modal {
    border: 1px solid rgba(124, 58, 237, 0.3);
    /* Borde sutil morado */
    box-shadow: 0 0 30px rgba(0, 0, 0, 0.8);
    border-radius: 16px !important;
    font-family: 'Inter', system-ui, sans-serif;
    /* Fuente limpia */
}

/* --- Título --- */
.void-title {
    font-weight: 700 !important;
    font-size: 1.8rem !important;
    letter-spacing: -0.5px;
}

/* --- Texto del cuerpo --- */
.void-text {
    color: #a1a1aa !important;
    /* Gris claro suave */
    font-size: 1.1rem !important;
}

/* --- El Botón (Estilo "AimAssist Active") --- */
.void-button {
    background: linear-gradient(135deg, #8b5cf6 0%, #6d28d9 100%);
    /* Gradiente morado */
    color: white;
    padding: 12px 32px;
    border-radius: 12px;
    /* Bordes redondeados como en la imagen */
    border: none;
    font-weight: 600;
    font-size: 1rem;
    cursor: pointer;
    box-shadow: 0 4px 15px rgba(139, 92, 246, 0.4);
    /* Resplandor (Glow) */
    transition: all 0.3s ease;
    outline: none;
}

.void-button:hover {
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(139, 92, 246, 0.6);
}

.void-button:active {
    transform: translateY(0);
}

/* --- Personalización del Icono (Opcional: Cambiarlo a Morado) --- */
/* Por defecto es verde. Esto lo fuerza a ser morado para combinar con tu UI */
.void-icon.swal2-success {
    border-color: #8b5cf6 !important;
    /* Anillo morado */
}

.void-icon.swal2-success [class^='swal2-success-line'] {
    background-color: #8b5cf6 !important;
    /* Líneas del check moradas */
}

.void-icon.swal2-success .swal2-success-ring {
    border: 4px solid rgba(139, 92, 246, 0.2) !important;
    /* Anillo suave */
}
</style>