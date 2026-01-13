<script setup lang="ts">
import { ref, computed, watch } from 'vue';

export interface SelectOption {
    value: string;
    label: string;
}

const props = withDefaults(defineProps<{
    modelValue?: (string)[];
    options?: SelectOption[];
    placeholder?: string;
    valueBlackList?: string[];
}>(), {
    modelValue: () => [],
    options: () => [],
    placeholder: 'conditions'
});

const emits = defineEmits<{
    (e: 'update:modelValue', value: (string)[]): void;
    (e: 'change', value1: (string)[], value2: (string)[]): void;
}>();

const isOpen = ref(false);
const selectedValues = ref<(string)[]>([...props.modelValue]);
const selectedValuesInBlackList = ref<(string)[]>([...props.modelValue]);

watch(() => props.modelValue, (newValue) => {
    selectedValues.value = [...(newValue || [])];
    console.log(newValue)
}, { deep: true });

const selectedLabels = computed(() => {
    return selectedValues.value
        .map(val => props.options.find(opt => opt.value === val)?.label || val)
        .join(', ');
});

const displayText = computed(() => {
    if (selectedValues.value.length === 0) {
        return props.placeholder;
    }
    if (selectedValues.value.length === 1) {
        return props.options.find(opt => opt.value === selectedValues.value[0])?.label || selectedValues.value[0];
    }
    return `${selectedValues.value.length} selected options`;
});

const toggleOption = (value: string) => {
    const index1 = selectedValues.value.indexOf(value);
    const index2 = selectedValuesInBlackList.value.indexOf(value);

    if (index2 > -1) {
        selectedValuesInBlackList.value.splice(index2, 1);
    } else {
        if (!props.valueBlackList?.some((opt: string) => opt == value)) {
            selectedValuesInBlackList.value.push(value);
        }
    }

    if (index1 > -1) {
        selectedValues.value.splice(index1, 1);
    } else {
        selectedValues.value.push(value);
    }

    emits('update:modelValue', [...selectedValues.value]);
    emits('change', [...selectedValues.value], [...selectedValuesInBlackList.value]);
};

const isSelected = (value: string) => {
    return selectedValues.value.includes(value);
};

const handleClickOutside = (event: MouseEvent) => {
    const target = event.target as HTMLElement;
    if (!target.closest('.custom-select-wrapper')) {
        isOpen.value = false;
    }
};

watch(isOpen, (open) => {
    if (open) {
        document.addEventListener('click', handleClickOutside);
    } else {
        document.removeEventListener('click', handleClickOutside);
    }
});
</script>

<template>
    <div class="custom-select-wrapper">
        <div class="custom-select" :class="{ 'is-open': isOpen, 'has-selection': selectedValues.length > 0 }"
            @click="isOpen = !isOpen">
            <span class="select-text" :class="{ 'placeholder': selectedValues.length === 0 }">
                {{ displayText }}
            </span>
            <div class="select-indicator" :class="{ 'is-open': isOpen }">
                <svg width="12" height="12" viewBox="0 0 12 12" fill="none">
                    <path d="M2 4L6 8L10 4" stroke="currentColor" stroke-width="2" stroke-linecap="round"
                        stroke-linejoin="round" />
                </svg>
            </div>
        </div>

        <div v-if="selectedValues.length > 0 && selectedLabels" class="selected-tags">
            <span class="tags-text select-none">{{ selectedLabels }}</span>
        </div>

        <Transition name="dropdown">
            <div v-if="isOpen" class="dropdown-menu">
                <div v-for="(option, index) in options" :key="index" class="dropdown-option"
                    :class="{ 'is-selected': isSelected(option.value) }" @click.stop="toggleOption(option.value)">
                    <div class="option-checkbox">
                        <div class="checkbox-inner" :class="{ 'checked': isSelected(option.value) }">
                            <svg v-if="isSelected(option.value)" width="10" height="10" viewBox="0 0 10 10" fill="none">
                                <path d="M1 5L4 8L9 1" stroke="currentColor" stroke-width="2" stroke-linecap="round"
                                    stroke-linejoin="round" />
                            </svg>
                        </div>
                    </div>
                    <span class="option-label">{{ option.label }}</span>
                </div>
                <div v-if="options.length === 0" class="dropdown-empty">
                    No hay opciones disponibles
                </div>
            </div>
        </Transition>
    </div>
</template>

<style scoped>
.custom-select-wrapper {
    position: relative;
    width: 100%;
}

.custom-select {
    width: 100%;
    padding: 0.625rem 0.875rem 0.625rem 0.875rem;
    background-color: rgba(35, 39, 50, 0.6);
    border: 1px solid rgba(99, 102, 241, 0.2);
    border-radius: 0.5rem;
    color: #e2e8f0;
    font-size: 0.875rem;
    cursor: pointer;
    outline: none;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    backdrop-filter: blur(8px);
    display: flex;
    align-items: center;
    justify-content: space-between;
    min-height: 2.5rem;
}

.custom-select:hover {
    border-color: rgba(99, 102, 241, 0.4);
    background-color: rgba(35, 39, 50, 0.8);
    box-shadow: 0 0 0 3px rgba(99, 102, 241, 0.1);
}

.custom-select.is-open {
    border-color: #6366F1;
    background-color: rgba(35, 39, 50, 0.9);
    box-shadow: 0 0 0 3px rgba(99, 102, 241, 0.2);
}

.custom-select:active {
    transform: scale(0.98);
}

.select-text {
    flex: 1;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
    transition: color 0.3s ease;
}

.select-text.placeholder {
    color: #64748B;
}

.select-indicator {
    display: flex;
    align-items: center;
    justify-content: center;
    color: #6366F1;
    transition: transform 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    flex-shrink: 0;
    margin-left: 0.5rem;
}

.select-indicator.is-open {
    transform: rotate(180deg);
}

.custom-select:hover .select-indicator {
    color: #818cf8;
}

.dropdown-menu {
    background-color: rgba(14, 16, 18, 0.95);
    border: 1px solid rgba(99, 102, 241, 0.3);
    border-radius: 0.5rem;
    box-shadow: 0 10px 25px rgba(0, 0, 0, 0.5);
    backdrop-filter: blur(12px);
    z-index: 99999;
    max-height: 200px;
    overflow-y: auto;
    margin-top: 0.25rem;
}

.dropdown-menu::-webkit-scrollbar {
    width: 6px;
}

.dropdown-menu::-webkit-scrollbar-track {
    background: rgba(35, 39, 50, 0.3);
    border-radius: 3px;
}

.dropdown-menu::-webkit-scrollbar-thumb {
    background: rgba(99, 102, 241, 0.5);
    border-radius: 3px;
}

.dropdown-menu::-webkit-scrollbar-thumb:hover {
    background: rgba(99, 102, 241, 0.7);
}

.dropdown-option {
    display: flex;
    align-items: center;
    gap: 0.75rem;
    padding: 0.75rem 1rem;
    cursor: pointer;
    transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
    color: #e2e8f0;
}

.dropdown-option:hover {
    background-color: rgba(99, 102, 241, 0.15);
}

.dropdown-option.is-selected {
    background-color: rgba(99, 102, 241, 0.2);
    color: #818cf8;
}

.option-checkbox {
    display: flex;
    align-items: center;
    justify-content: center;
    flex-shrink: 0;
}

.checkbox-inner {
    width: 1.125rem;
    height: 1.125rem;
    border: 2px solid rgba(99, 102, 241, 0.4);
    border-radius: 0.25rem;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
    background-color: transparent;
    color: #6366F1;
}

.checkbox-inner.checked {
    background-color: #6366F1;
    border-color: #6366F1;
    color: #ffffff;
}

.option-label {
    flex: 1;
    user-select: none;
}

.dropdown-empty {
    padding: 1rem;
    text-align: center;
    color: #64748B;
    font-size: 0.875rem;
}

.selected-tags {
    margin-top: 0.5rem;
    padding: 0.5rem;
    background-color: rgba(35, 39, 50, 0.4);
    border-radius: 0.375rem;
    font-size: 0.75rem;
    color: #94a3b8;
    height: fit-content;
}

.tags-text {
    display: block;
    line-height: 1.4;
}

/* Animaciones */
.dropdown-enter-active,
.dropdown-leave-active {
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    transform-origin: top;
}

.dropdown-enter-from {
    opacity: 0;
    transform: translateY(-10px) scale(0.95);
}

.dropdown-enter-to {
    opacity: 1;
    transform: translateY(0) scale(1);
}

.dropdown-leave-from {
    opacity: 1;
    transform: translateY(0) scale(1);
}

.dropdown-leave-to {
    opacity: 0;
    transform: translateY(-10px) scale(0.95);
}

/* Compatibilidad con temas oscuros */
@media (prefers-color-scheme: dark) {
    .custom-select {
        background-color: rgba(35, 39, 50, 0.6);
        color: #e2e8f0;
    }

    .custom-select:hover {
        background-color: rgba(35, 39, 50, 0.8);
    }

    .custom-select.is-open {
        background-color: rgba(35, 39, 50, 0.9);
    }

    .dropdown-menu {
        background-color: rgba(14, 16, 18, 0.95);
    }
}
</style>
