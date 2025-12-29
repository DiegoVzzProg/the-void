<script setup lang="ts">
import {
    BugOff,
    Cpu,
    Shredder,
    Swords
} from 'lucide-vue-next';
import CardInformation from '../../components/CardInformation.vue';
import HeroFeature from '../../components/HeroFeature.vue';


</script>

<template>
    <header class="w-full h-16 flex flex-col justify-center border-b border-b-[#232732] mb-3">
        <span class="text-[clamp(18px,3vw,24px)]">welcome user</span>
    </header>
    <div class="flex flex-col flex-1 lg:px-2 gap-2 overflow-auto">
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
        <div class="grid grid-cols-1">
            <CardInformation>
                <div class="flex items-start gap-4">
                    <div>
                        <h3 class="text-lg font-bold text-white group-hover:text-indigo-200 transition-colors">
                            The Void (Basic)
                        </h3>
                        <p class="mt-2 text-sm leading-relaxed text-gray-400">
                            Timeless architecture. Universal modules built to outlast any anticheat update.
                        </p>
                    </div>
                </div>
            </CardInformation>
        </div>
    </div>
</template>

<style scoped></style>