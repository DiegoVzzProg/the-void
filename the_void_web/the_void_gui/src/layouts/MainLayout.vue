<script setup lang="ts">

</script>

<template>
    <!-- Orbe 1: Brillo Teal (Arriba Izquierda) -->
    <div class="absolute top-[-10%] left-[-10%] w-[500px] h-[500px] 
              bg-[#2dd4bf]/10 rounded-full blur-[120px] 
              pointer-events-none animate-pulse">
    </div>

    <!-- Orbe 2: Brillo Rosa (Abajo Derecha con Retraso) -->
    <div class="absolute bottom-[-10%] right-[-10%] w-[500px] h-[500px] 
              bg-[#f472b6]/10 rounded-full blur-[120px] 
              pointer-events-none animate-pulse" style="animation-delay: 1s;">
    </div>

    <main class="flex flex-col w-full h-dvh overflow-auto">
        <slot></slot>
    </main>
</template>

<style scoped></style>
