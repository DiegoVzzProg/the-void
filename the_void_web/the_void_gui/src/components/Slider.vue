<script setup lang="ts">
import { computed } from 'vue';

const props = defineProps<{ modelValue?: number, min?: number, max?: number }>();
const emits = defineEmits<{ (e: 'update:modelValue', value: number): void, (e: 'change', value: number): void }>();

const innerValue = computed({
    get: () => props.modelValue ?? 10,
    set: (val: number) => emits('update:modelValue', val)
});

const minValue = computed(() => props.min ?? 1);
const maxValue = computed(() => props.max ?? 100);

const percentage = computed(() => {
    const range = maxValue.value - minValue.value;
    return range > 0 ? ((innerValue.value - minValue.value) / range) * 100 : 0;
});
</script>

<template>
    <div class="w-full py-2">
        <div class="relative w-full h-1.5 bg-[#1f2029] rounded-full">
            <div class="absolute h-full bg-[#6366f1] rounded-full pointer-events-none shadow-[0_0_10px_rgba(99,102,241,0.6)]"
                :style="{ width: percentage + '%' }"></div>

            <input type="range" :min="minValue" :max="maxValue" v-model.number="innerValue"
                @change="(e) => emits('change', Number((e.target as HTMLInputElement).value))"
                class="absolute w-full h-full opacity-0 cursor-pointer z-10" />

            <div class="absolute top-1/2 -translate-y-1/2 w-3.5 h-3.5 bg-white rounded-full pointer-events-none shadow-md transition-transform duration-75 active:scale-125"
                :style="{ left: `calc(${percentage}% - 7px)` }">
            </div>
        </div>
    </div>
</template>

<style scoped>
/* From Uiverse.io by Galahhad */
/* level settings 👇 */

.slider {
    /* slider */
    --slider-width: 100%;
    --slider-height: 20px;
    --slider-bg: rgb(82, 82, 82);
    --slider-border-radius: 4px;
    /* level */
    --level-color: #6366F1;
    --level-transition-duration: .1s;
    /* icon */
    --icon-margin: 15px;
    --icon-color: var(--slider-bg);
    --icon-size: 25px;
    width: 100%;
    min-width: 0;
}

.slider {
    cursor: pointer;
    display: -webkit-inline-box;
    display: -ms-inline-flexbox;
    display: inline-flex;
    -webkit-box-orient: horizontal;
    -webkit-box-direction: reverse;
    -ms-flex-direction: row-reverse;
    flex-direction: row-reverse;
    -webkit-box-align: center;
    -ms-flex-align: center;
    align-items: center;
    width: 100%;
}

.slider .volume {
    display: inline-block;
    vertical-align: top;
    margin-right: var(--icon-margin);
    color: var(--icon-color);
    width: var(--icon-size);
    height: auto;
}

.slider .level {
    -webkit-appearance: none;
    -moz-appearance: none;
    appearance: none;
    width: var(--slider-width);
    height: var(--slider-height);
    background: var(--slider-bg);
    overflow: hidden;
    border-radius: var(--slider-border-radius);
    -webkit-transition: height var(--level-transition-duration);
    -o-transition: height var(--level-transition-duration);
    transition: height var(--level-transition-duration);
    cursor: inherit;
    width: 100%;
    min-width: 0;
}

.slider .level::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 0;
    height: 0;
    box-shadow: -1000px 0 0 1000px var(--level-color);
    -webkit-box-shadow: -1000px 0 0 1000px var(--level-color);
}

.slider .level::-moz-range-thumb {
    -moz-appearance: none;
    width: 0;
    height: 0;
    border: none;
    box-shadow: -1000px 0 0 1000px var(--level-color);
}

.slider .level::-ms-thumb {
    width: 0;
    height: 0;
    box-shadow: -1000px 0 0 1000px var(--level-color);
}
</style>