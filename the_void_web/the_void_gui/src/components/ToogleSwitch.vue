<script setup lang="ts">
import { ref, watch } from 'vue';


// Soporte para v-model, checked y events
const props = defineProps<{
    modelValue?: boolean,
    checked?: boolean
}>();

const emits = defineEmits<{
    (e: 'update:modelValue', value: boolean): void,
    (e: 'change', event: Event): void
}>();

// Estado interno (espejo de v-model/checked)
const internalValue = ref(
    props.modelValue !== undefined
        ? props.modelValue
        : props.checked !== undefined
            ? props.checked
            : false
);

// Refleja cambios de padre
watch(
    () => props.modelValue,
    (newVal) => {
        if (newVal !== undefined) internalValue.value = newVal;
    }
);

watch(
    () => props.checked,
    (newVal) => {
        if (props.modelValue === undefined && newVal !== undefined)
            internalValue.value = newVal;
    }
);

// Cuando cambia en UI
function onChange(event: Event) {
    const target = event.target as HTMLInputElement;
    internalValue.value = target.checked;

    emits('update:modelValue', target.checked); // v-model compatible
    emits('change', event); // Para v-on:change desde padre
}
</script>

<template>
    <label class="switch relative inline-block w-12 h-6">
        <input type="checkbox" class="opacity-0 w-0 h-0" :checked="internalValue" @change="onChange">
        <span
            class="slider round absolute cursor-pointer top-0 left-0 right-0 bottom-0 bg-[#1a1b26] border border-white/10 transition-all duration-300 rounded-full"></span>
    </label>
</template>

<style scoped>
/* El círculo (thumb) */
.slider:before {
    position: absolute;
    content: "";
    height: 16px;
    width: 16px;
    left: 4px;
    bottom: 3px;
    background-color: #52525b;
    /* Zinc-600 inactivo */
    transition: .3s;
    border-radius: 50%;
}

/* Estado Activo: Fondo Morado Oscuro + Borde Brillante */
input:checked+.slider {
    background-color: #312e81;
    /* Indigo-900 */
    border-color: #6366f1;
    /* Indigo-500 */
    box-shadow: 0 0 10px rgba(99, 102, 241, 0.3);
}

/* Estado Activo: Círculo Blanco Brillante */
input:checked+.slider:before {
    background-color: #ffffff;
    transform: translateX(22px);
    box-shadow: 0 0 5px rgba(255, 255, 255, 0.8);
}
</style>
