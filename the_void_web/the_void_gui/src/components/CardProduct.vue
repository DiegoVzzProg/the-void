<script setup lang="ts">
import { ShoppingBag, Check, Zap, Star, Clock } from 'lucide-vue-next';

interface ProductProps {
    title: string;
    subtitle?: string;
    price: string;
    period?: string;
    features?: string[];
    isPopular?: boolean;
    isComingSoon?: boolean;
}

const props = defineProps<ProductProps>();
const emit = defineEmits(['buy']);
</script>

<template>
    <div class="group relative flex flex-col w-full h-full transition-all duration-500 ease-out hover:-translate-y-2">

        <div class="relative flex flex-col h-full rounded-4xl overflow-hidden backdrop-blur-xl border flex-1 transition-all duration-500 ease-out"
            :class="[
                props.isPopular
                    ? 'bg-[#0b0c15]/80 border-indigo-500/10 shadow-2xl shadow-indigo-500/5'
                    : 'bg-[#0e1016]/40 border-white/5 hover:bg-[#0e1016]/60 hover:border-white/10'
            ]">

            <!-- Popular Glow -->
            <div v-if="props.isPopular"
                class="absolute inset-0 rounded-4xl border border-indigo-500/10 pointer-events-none z-10 transition-colors duration-500">
            </div>

            <div
                class="p-6 md:p-8 flex flex-col gap-4 border-b border-white/5 relative bg-white/1 transition-colors duration-500">

                <!-- Badges Container -->
                <div class="absolute top-6 right-6 flex flex-col items-end gap-2">
                    <div v-if="props.isPopular"
                        class="inline-flex items-center gap-1.5 rounded-full bg-indigo-500/10 border border-indigo-500/20 px-3 py-1 text-[10px] font-bold uppercase tracking-widest text-indigo-300/80 shadow-[0_0_15px_rgba(99,102,241,0.15)] transition-all duration-500 group-hover:bg-indigo-500/20">
                        <Star class="w-3 h-3 fill-indigo-300/80 text-indigo-300" />
                        Popular
                    </div>
                    <div v-if="props.isComingSoon"
                        class="inline-flex items-center gap-1.5 rounded-full bg-amber-500/5 border border-amber-500/10 px-3 py-1 text-[10px] font-bold uppercase tracking-widest text-amber-300/90 shadow-[0_0_15px_rgba(245,158,11,0.1)] transition-all duration-500 group-hover:bg-amber-500/10">
                        <Clock class="w-3 h-3 text-amber-300/90" />
                        Coming Soon
                    </div>
                </div>

                <div>
                    <h3
                        class="text-lg md:text-xl font-bold text-white/90 tracking-tight flex items-center gap-2 group-hover:text-white transition-colors duration-500">
                        {{ props.title }}
                        <Zap v-if="props.isPopular" class="w-4 h-4 text-yellow-400/80 fill-yellow-400/80" />
                    </h3>
                    <p
                        class="text-sm text-gray-400 mt-1 font-medium transition-colors duration-500 group-hover:text-gray-300">
                        {{ props.subtitle }}</p>
                </div>

                <div class="flex items-end gap-1 mt-2">
                    <span
                        class="text-3xl md:text-4xl font-bold text-white/90 tracking-tight leading-none transition-colors duration-500 group-hover:text-white"
                        :class="{ 'text-white/40': props.isComingSoon }">
                        {{ props.price }}
                    </span>
                    <span v-if="props.period"
                        class="text-sm font-medium text-gray-500 mb-1.5 group-hover:text-gray-400 transition-colors duration-500">
                        {{ props.period }}
                    </span>
                </div>
            </div>

            <div class="p-6 md:p-8 flex flex-col flex-1">
                <ul class="space-y-4 mb-8 flex-1">
                    <li v-for="(feature, index) in props.features" :key="index"
                        class="flex items-start gap-3 group/item">
                        <div class="mt-0.5 flex h-5 w-5 shrink-0 items-center justify-center rounded-full transition-all duration-500 ease-out"
                            :class="[
                                props.isPopular
                                    ? 'bg-indigo-500/10 text-indigo-400/80 group-hover/item:bg-indigo-500/20 group-hover/item:text-indigo-300'
                                    : 'bg-white/5 text-gray-500 group-hover/item:bg-white/10 group-hover/item:text-gray-300',
                                props.isComingSoon && !props.isPopular ? 'grayscale opacity-40' : ''
                            ]">
                            <Check class="h-3 w-3" stroke-width="4" />
                        </div>
                        <span
                            class="text-sm text-gray-400 font-medium leading-relaxed transition-colors duration-500 group-hover/item:text-gray-300"
                            :class="{ 'text-gray-600': props.isComingSoon }">
                            {{ feature }}
                        </span>
                    </li>
                </ul>

                <!-- Button -->
                <button @click="!props.isComingSoon && emit('buy')" :disabled="props.isComingSoon"
                    class="w-full relative overflow-hidden rounded-xl py-4 text-sm font-bold transition-all duration-500 ease-out active:scale-[0.98] mt-auto"
                    :class="[
                        props.isComingSoon
                            ? 'bg-amber-500/5 text-amber-500/70 border border-amber-500/10 select-none'
                            : props.isPopular
                                ? 'bg-indigo-600 text-white shadow-lg shadow-indigo-500/20 hover:bg-indigo-500 hover:shadow-indigo-500/30'
                                : 'bg-white/5 text-white/90 border border-white/5 hover:bg-white/10 hover:border-white/10'
                    ]">
                    <div v-if="props.isPopular && !props.isComingSoon"
                        class="absolute inset-0 -translate-x-full group-hover:animate-[shimmer_2s_infinite] bg-linear-to-r from-transparent via-white/10 to-transparent z-0">
                    </div>

                    <span class="relative z-10 flex items-center justify-center gap-2">
                        <template v-if="props.isComingSoon">
                            Coming Soon
                            <Clock class="w-4 h-4" />
                        </template>
                        <template v-else>
                            Get Started
                            <ShoppingBag class="w-4 h-4 transition-transform group-hover:translate-x-1" />
                        </template>
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