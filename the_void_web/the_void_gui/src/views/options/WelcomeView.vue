<script setup lang="ts">
import {
    BugOff,
    Cpu,
    Shredder,
    Swords
} from 'lucide-vue-next';
import CardInformation from '../../components/CardInformation.vue';
import HeroFeature from '../../components/HeroFeature.vue';
import CardProduct from '../../components/CardProduct.vue';

const products = [
    {
        title: "The Void (AutoClicker)",
        subtitle: "Essential clicking automation",
        price: "$9.99",
        period: "/month",
        features: [
            "Undetected Clicker Logic",
            "Randomization Control",
            "Basic Support",
            "Windows 10/11 Compatible"
        ],
        isPopular: false
    },
    {
        title: "The Void",
        subtitle: "Complete combat suite",
        price: "$24.99",
        period: "/month",
        features: [
            "Access to All Modules",
            "AimAssist + Reach + Velocity",
            "Trace Removal Protocol",
            "Priority 24/7 Support",
            "Early Access to Updates"
        ],
        isPopular: true
    }
];

const handleBuy = (productTitle: string) => {
    console.log(`Comprando: ${productTitle}`);
    // Aquí tu lógica de redirección a pasarela de pago
};

</script>

<template>
    <header class="w-full min-h-16 flex items-center mb-3">
        <h1 class="text-2xl font-bold text-white tracking-tight flex items-center gap-2">
            <span class="w-2 h-6 bg-[#6366f1] rounded-full"></span>
            information
        </h1>
    </header>

    <div class="grid grid-cols-1 gap-4 lg:gap-6 overflow-auto h-[calc(100dvh-186px)]">
        <HeroFeature />

        <span class="w-full text-center py-4 text-[24px] font-semibold">Why choose us?</span>
        <div class="grid grid-cols-1 gap-4 lg:grid-cols-2">

            <CardInformation>
                <div class="flex items-start gap-4">
                    <div
                        class="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 transition-colors group-hover:bg-indigo-500/20">
                        <Swords class="h-6 w-6 text-indigo-400" />
                    </div>

                    <div>
                        <h3 class="text-lg font-bold text-white group-hover:text-indigo-200 transition-colors">
                            Humanized Combat Logic
                        </h3>
                        <p class="mt-2 text-sm leading-relaxed text-gray-400">
                            Timeless architecture. Universal modules built to outlast any anticheat update.
                        </p>
                    </div>
                </div>
            </CardInformation>

            <CardInformation>
                <div class="flex items-start gap-4">
                    <div
                        class="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 transition-colors group-hover:bg-indigo-500/20">
                        <Cpu class="h-6 w-6 text-indigo-400" />
                    </div>

                    <div>
                        <h3 class="text-lg font-bold text-white group-hover:text-indigo-200 transition-colors">
                            Performance
                        </h3>
                        <p class="mt-2 text-sm leading-relaxed text-gray-400">
                            Native C++ performance controlled from the shadows. Decoupled interface ensuring a
                            zero-footprint experience.
                        </p>
                    </div>
                </div>
            </CardInformation>

            <CardInformation class="lg:col-span-2">
                <div class="flex items-start gap-4">
                    <div
                        class="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 transition-colors group-hover:bg-indigo-500/20">
                        <Shredder class="h-6 w-6 text-indigo-400" />
                    </div>

                    <div>
                        <h3 class="text-lg font-bold text-white group-hover:text-indigo-200 transition-colors">
                            Trace Removal Protocol
                        </h3>
                        <p class="mt-2 text-sm leading-relaxed text-gray-400 max-w-2xl">
                            Instant memory wipe. Cleans strings and prefetch data to leave zero forensic evidence on the
                            target machine.
                        </p>
                    </div>
                </div>
            </CardInformation>

            <CardInformation class="lg:col-span-2">
                <div class="flex items-start gap-4">
                    <div
                        class="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 transition-colors group-hover:bg-indigo-500/20">
                        <BugOff class="h-6 w-6 text-indigo-400" />
                    </div>

                    <div>
                        <h3 class="text-lg font-bold text-white group-hover:text-indigo-200 transition-colors">
                            Active support
                        </h3>
                        <p class="mt-2 text-sm leading-relaxed text-gray-400 max-w-2xl">
                            Rest easy knowing our dedicated team has your back. We are here for you 24/7 to ensure you
                            absolutely love your purchase.
                        </p>
                    </div>
                </div>
            </CardInformation>

        </div>

        <span class="w-full text-center py-4 text-[24px] font-semibold">Product</span>
        <div class="grid grid-cols-1 lg:grid-cols-2 gap-2" id="prod_str">
            <CardProduct v-for="product in products" :key="product.title" v-bind="product"
                @buy="handleBuy(product.title)" />
        </div>
    </div>
</template>

<style scoped></style>