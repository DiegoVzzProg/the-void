<script setup lang="ts">
import { computed } from "vue";
import { useRoute } from "vue-router";
import MainLayout from "./layouts/MainLayout.vue";

const route = useRoute();
const layout = computed(() => {
  return route.meta.layout === "Main" ? MainLayout : null;
});
</script>

<template>
  <component :is="layout">
    <router-view></router-view>
  </component>
</template>

<style scoped></style>
