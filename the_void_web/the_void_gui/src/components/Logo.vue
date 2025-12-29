<script setup lang="ts">
const props = withDefaults(defineProps<{
    fontSizeLogo?: string,
    sloganHidde?: boolean
}>(), {
    fontSizeLogo: 'clamp(18px,3vw,24px)',
    sloganHidde: false
});
</script>

<template>
    <div class="flex flex-col items-start leading-[1.1]">
        <span :class="`gradient-text text-[${props.fontSizeLogo}] font-semibold select-none`">
            the void
        </span>
        <span v-if="!props.sloganHidde" class="text-[12px] text-[#64748B] google-sans-code select-none">
            web_client v1.0
        </span>
    </div>
</template>

<style scoped></style>