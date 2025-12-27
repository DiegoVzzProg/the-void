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
    <label class="switch">
        <input type="checkbox" :checked="internalValue" @change="onChange">
        <span class="slider"></span>
    </label>
</template>

<style scoped>
.switch {
    --secondary-container: #3a4b39;
    --primary: #84da89;
    font-size: 17px;
    position: relative;
    display: inline-block;
    width: 3.7em;
    height: 1.8em;
}

.switch input {
    display: none;
    opacity: 0;
    width: 0;
    height: 0;
}

.slider {
    position: absolute;
    cursor: pointer;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: #313033;
    transition: .2s;
    border-radius: 30px;
}

.slider:before {
    position: absolute;
    content: "";
    height: 1.4em;
    width: 1.4em;
    border-radius: 20px;
    left: 0.2em;
    bottom: 0.2em;
    background-color: #aeaaae;
    transition: .4s;
}

input:checked+.slider::before {
    background-color: var(--primary);
}

input:checked+.slider {
    background-color: var(--secondary-container);
}

input:focus+.slider {
    box-shadow: 0 0 1px var(--secondary-container);
}

input:checked+.slider:before {
    transform: translateX(1.9em);
}
</style>
