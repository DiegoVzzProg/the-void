<script setup lang="ts">
import { ShoppingBag, Check, Zap, Star } from 'lucide-vue-next';

interface ProductProps {
    title: string;
    subtitle?: string;
    price: string;
    period?: string;
    features?: string[];
    isPopular?: boolean;
}

const props = defineProps<ProductProps>();
const emit = defineEmits(['buy']);
</script>

<template>
    <div class="group relative flex flex-col w-full h-full transition-all duration-300 hover:-translate-y-1">

        <div class="relative flex flex-col h-full rounded-[2rem] overflow-hidden backdrop-blur-md border flex-1" :class="[
            props.isPopular
                ? 'bg-[#0b0c15] border-transparent'
                : 'bg-[#0e1016]/80 border-white/5 hover:border-white/10'
        ]">

            <div v-if="props.isPopular"
                class="absolute inset-0 rounded-[2rem] border border-white/10 pointer-events-none z-10"></div>

            <div class="p-6 md:p-8 flex flex-col gap-4 border-b border-white/5 relative bg-white/[0.02]">

                <div v-if="props.isPopular" class="absolute top-6 right-6">
                    <div
                        class="inline-flex items-center gap-1.5 rounded-full bg-indigo-500/20 border border-indigo-500/30 px-3 py-1 text-[10px] font-bold uppercase tracking-widest text-indigo-300 shadow-[0_0_10px_rgba(99,102,241,0.3)]">
                        <Star class="w-3 h-3 fill-indigo-300" />
                        Popular
                    </div>
                </div>

                <div>
                    <h3 class="text-lg md:text-xl font-bold text-white tracking-tight flex items-center gap-2">
                        {{ props.title }}
                        <Zap v-if="props.isPopular" class="w-4 h-4 text-yellow-400 fill-yellow-400" />
                    </h3>
                    <p class="text-sm text-gray-400 mt-1 font-medium">{{ props.subtitle }}</p>
                </div>

                <div class="flex items-end gap-1 mt-2">
                    <span class="text-3xl md:text-4xl font-bold text-white tracking-tight leading-none">
                        {{ props.price }}
                    </span>
                    <span v-if="props.period" class="text-sm font-medium text-gray-500 mb-1.5">
                        {{ props.period }}
                    </span>
                </div>
            </div>

            <div class="p-6 md:p-8 flex flex-col flex-1">
                <ul class="space-y-4 mb-8 flex-1">
                    <li v-for="(feature, index) in props.features" :key="index"
                        class="flex items-start gap-3 group/item">
                        <div class="mt-0.5 flex h-5 w-5 shrink-0 items-center justify-center rounded-full transition-colors duration-300"
                            :class="props.isPopular ? 'bg-indigo-500/20 text-indigo-400 group-hover/item:bg-indigo-500 group-hover/item:text-white' : 'bg-white/5 text-gray-400 group-hover/item:bg-white/10 group-hover/item:text-white'">
                            <Check class="h-3 w-3" stroke-width="4" />
                        </div>
                        <span class="text-sm text-gray-300 font-medium leading-relaxed">{{ feature }}</span>
                    </li>
                </ul>

                <button @click="emit('buy')"
                    class="w-full relative overflow-hidden rounded-xl py-4 text-sm font-bold transition-all duration-300 active:scale-[0.98] mt-auto"
                    :class="[
                        props.isPopular
                            ? 'bg-indigo-600 text-white shadow-lg shadow-indigo-500/25 hover:bg-indigo-500 hover:shadow-indigo-500/40'
                            : 'bg-white/5 text-white border border-white/5 hover:bg-white/10 hover:border-white/20'
                    ]">
                    <div v-if="props.isPopular"
                        class="absolute inset-0 -translate-x-full group-hover:animate-[shimmer_1.5s_infinite] bg-gradient-to-r from-transparent via-white/20 to-transparent z-0">
                    </div>

                    <span class="relative z-10 flex items-center justify-center gap-2">
                        Get Started
                        <ShoppingBag class="w-4 h-4 transition-transform group-hover:translate-x-1" />
                    </span>
                </button>
            </div>
        </div>
    </div>
</template>

<style scoped>
/* Animación para el brillo del botón */
@keyframes shimmer {
    100% {
        transform: translateX(100%);
    }
}
</style>