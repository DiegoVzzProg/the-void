<script setup lang="ts">
import {
    BugOff,
    Shredder,
    Swords,
    Sparkles,
    Box,
    Cloud
} from 'lucide-vue-next';
import CardInformation from '../../components/CardInformation.vue';
import HeroFeature from '../../components/HeroFeature.vue';
import CardProduct from '../../components/CardProduct.vue';

const products = [
    {
        title: "Void: Cloud Clicker",
        subtitle: "The only thing they'll see is your victory.",
        price: "$5.99",
        period: "/month",
        features: [
            "Cloud GUI Access",
            "Mobile Control",
            "24/7 Priority Support",
            "Instant Algorithm Updates",
            "Windows 10/11 Compatible"
        ],
        isPopular: true
    },
    {
        title: "Void: Cloud Combat",
        subtitle: "Complete combat suite",
        price: "$20.00",
        period: "/month",
        features: [
            "Access to All Modules",
            "Trace Removal Protocol",
            "Priority 24/7 Support",
            "Early Access to Updates",
            "Windows 10/11 Compatible"
        ],
        isComingSoon: true
    }
];

const handleBuy = (productTitle: string) => {
    console.log(`Comprando: ${productTitle}`);
    // Aquí tu lógica de redirección a pasarela de pago
};

</script>

<template>
    <div class="flex flex-col h-full overflow-y-auto overflow-x-hidden pr-2">
        <header class="w-full min-h-16 flex items-center mb-6 shrink-0">
            <h1 class="text-3xl font-bold text-white tracking-tight flex items-center gap-3">
                <span
                    class="w-1.5 h-8 bg-linear-to-b from-indigo-500 to-purple-600 rounded-full shadow-[0_0_15px_rgba(99,102,241,0.5)]"></span>
                <span class="bg-clip-text text-transparent bg-linear-to-r from-white to-gray-400">information</span>
            </h1>
        </header>

        <div class="flex flex-col gap-12 pb-10">
            <!-- Hero Section -->
            <section>
                <HeroFeature />
            </section>

            <!-- Features Section (Bento Grid) -->
            <section class="flex flex-col gap-6">
                <div class="flex items-center gap-3 mb-2">
                    <Sparkles class="w-5 h-5 text-indigo-400" />
                    <h2 class="text-xl font-bold text-white tracking-wide">Why choose us?</h2>
                    <div class="h-px bg-white/10 flex-1"></div>
                </div>

                <div class="grid grid-cols-1 lg:grid-cols-2 gap-4">
                    <CardInformation
                        class="bg-[#0e1016]/80 hover:bg-[#0e1016] border border-white/5 hover:border-indigo-500/20 transition-all duration-300">
                        <div class="flex items-start gap-5">
                            <div
                                class="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                                <Swords class="h-6 w-6 text-indigo-400" />
                            </div>
                            <div>
                                <h3 class="text-lg font-bold text-white mb-1">Humanized Combat Logic</h3>
                                <p class="text-sm leading-relaxed text-gray-400">
                                    Timeless architecture. Universal modules built to outlast any anticheat update.
                                </p>
                            </div>
                        </div>
                    </CardInformation>

                    <CardInformation
                        class="bg-[#0e1016]/80 hover:bg-[#0e1016] border border-white/5 hover:border-indigo-500/20 transition-all duration-300">
                        <div class="flex items-start gap-5">
                            <div
                                class="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                                <Cloud class="h-6 w-6 text-indigo-400" />
                            </div>
                            <div>
                                <h3 class="text-lg font-bold text-white mb-1">Performance</h3>
                                <p class="text-sm leading-relaxed text-gray-400">
                                    Native C++ performance controlled from the cloud. Decoupled interface for a seamless
                                    experience.
                                </p>
                            </div>
                        </div>
                    </CardInformation>

                    <CardInformation
                        class="lg:col-span-2 bg-[#0e1016]/80 hover:bg-[#0e1016] border border-white/5 hover:border-indigo-500/20 transition-all duration-300">
                        <div class="flex items-start gap-5">
                            <div
                                class="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                                <Shredder class="h-6 w-6 text-indigo-400" />
                            </div>
                            <div>
                                <h3 class="text-lg font-bold text-white mb-1">Trace Removal Protocol</h3>
                                <p class="text-sm leading-relaxed text-gray-400 max-w-3xl">
                                    Instant memory wipe. Cleans strings and prefetch data to leave zero forensic
                                    evidence on the target machine.
                                </p>
                            </div>
                        </div>
                    </CardInformation>

                    <CardInformation
                        class="lg:col-span-2 bg-[#0e1016]/80 hover:bg-[#0e1016] border border-white/5 hover:border-indigo-500/20 transition-all duration-300">
                        <div class="flex items-start gap-5">
                            <div
                                class="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                                <BugOff class="h-6 w-6 text-indigo-400" />
                            </div>
                            <div>
                                <h3 class="text-lg font-bold text-white mb-1">Active support</h3>
                                <p class="text-sm leading-relaxed text-gray-400 max-w-3xl">
                                    Rest easy knowing our dedicated team has your back. We are here for you 24/7 to
                                    ensure you absolutely love your purchase.
                                </p>
                            </div>
                        </div>
                    </CardInformation>
                </div>
            </section>

            <!-- Product Section -->
            <section class="flex flex-col gap-6">
                <div class="flex items-center gap-3 mb-2">
                    <Box class="w-5 h-5 text-indigo-400" />
                    <h2 class="text-xl font-bold text-white tracking-wide">Available Products</h2>
                    <div class="h-px bg-white/10 flex-1"></div>
                </div>

                <div class="grid grid-cols-1 lg:grid-cols-2 gap-6" id="prod_str">
                    <!-- If single product, limit width or span full? Keeping grid allows expansion. -->
                    <CardProduct v-for="product in products" :key="product.title" v-bind="product"
                        @buy="handleBuy(product.title)" />
                </div>
            </section>
        </div>
    </div>
</template>

<style scoped>
/* Scrollbar fino para container principal */
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