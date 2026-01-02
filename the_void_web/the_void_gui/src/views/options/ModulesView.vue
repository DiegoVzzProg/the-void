<script setup lang="ts">

import AutoClickerModuleView from './modules/AutoClickerModuleView.vue';
import ReachModuleView from './modules/ReachModuleView.vue';
import AimAssistModuleView from './modules/AimAssistModuleView.vue';
import VelocityModuleView from './modules/VelocityModuleView.vue';

</script>

<template>
    <header class="w-full min-h-16 flex items-center mb-3">
        <h1 class="text-2xl font-bold text-white tracking-tight flex items-center gap-2">
            <span class="w-2 h-6 bg-[#6366f1] rounded-full"></span>
            modules
        </h1>
    </header>

    <div class="grid grid-cols-1 md:grid-cols-2 gap-4 lg:gap-6 pb-10 overflow-auto h-[calc(100dvh-186px)]">
        <AutoClickerModuleView />

        <ReachModuleView />

        <AimAssistModuleView />

        <VelocityModuleView />
    </div>
</template>

<style scoped></style>