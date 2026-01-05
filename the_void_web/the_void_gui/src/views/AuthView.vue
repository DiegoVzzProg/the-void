<script setup lang="ts">
import { useRoute } from 'vue-router';
import LoginView from './auth/LoginView.vue';
import RegisterView from './auth/RegisterView.vue';

const router = useRoute();

</script>

<template>
    <header class="w-full min-h-16 flex items-center mb-3">
        <h1 class="text-2xl font-bold text-white tracking-tight flex items-center gap-2">
            <span class="w-2 h-6 bg-[#6366f1] rounded-full"></span>
            auth
        </h1>
    </header>
    <div class="flex flex-col flex-1 relative overflow-auto items-center">
        <LoginView v-if="router.params.view === 'login'" />
        <RegisterView v-else />
    </div>
</template>

<style scoped></style>