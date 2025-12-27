<script setup lang="ts">
import { computed } from 'vue';

// Soporte para v-model:
// - modelValue es el valor que viene/controla el padre
// - emits para update:modelValue y change
const props = defineProps<{
    modelValue?: number,
    min?: number,
    max?: number,
}>();

const emits = defineEmits<{
    (e: 'update:modelValue', value: number): void,
    (e: 'change', value: number): void
}>();

// Valor interno, reflejando la prop modelValue o fallback a un valor por defecto (10)
const innerValue = computed({
    get: () => props.modelValue !== undefined ? props.modelValue : 10,
    set: (val: number) => emits('update:modelValue', val)
});

const minValue = computed(() => props.min ?? 1);
const maxValue = computed(() => props.max ?? 100);

// Calcular el porcentaje para el efecto visual
const percentage = computed(() => {
    const range = maxValue.value - minValue.value;
    const value = innerValue.value - minValue.value;
    return range > 0 ? (value / range) * 100 : 0;
});

// Al mover el slider, también emitir 'change' hacia el padre
function onChange(event: Event) {
    const value = Number((event.target as HTMLInputElement).value);
    emits('change', value);
}
</script>

<template>
    <div class="flex w-full">
        <label class="slider w-full">
            <input
                type="range"
                class="level"
                :min="minValue"
                :max="maxValue"
                v-model.number="innerValue"
                :style="{ '--percentage': percentage + '%' }"
                @change="onChange"
            />
        </label>
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