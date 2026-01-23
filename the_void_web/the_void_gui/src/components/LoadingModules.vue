<script setup lang="ts">
import { onMounted, ref } from 'vue';

const lines = ref<{ text: string, status?: string, statusColor?: string }[]>([]);

const sequence = [
    { text: "> STARTING V.O.I.D. KERNEL...", delay: 200 },
    { text: "> [V]OLATILE...........", status: "OK", statusColor: "text-indigo-400", delay: 600 },
    { text: "> [O]BFUSCATED.......", status: "OK", statusColor: "text-indigo-400", delay: 1000 },
    { text: "> [I]NJECTED........", status: "ONLINE", statusColor: "text-purple-400 font-bold", delay: 2000 },
    { text: "> [D]ESTRUCTIBLE......", status: "OK", statusColor: "text-indigo-400", delay: 2340 },
];

onMounted(() => {
    sequence.forEach((item) => {
        setTimeout(() => {
            lines.value.push({
                text: item.text,
                status: item.status,
                statusColor: item.statusColor
            });
        }, item.delay);
    });
});
</script>

<template>
    <div class="flex flex-col items-center justify-center w-full h-full min-h-[300px] select-none">
        <div class="font-mono text-sm md:text-base rounded-xl p-8 min-w-[320px]">
            <div class="flex flex-col gap-2">
                <template v-for="(line, index) in lines" :key="index">
                    <div class="flex justify-between gap-8 animate-fade-in">
                        <span class="text-gray-300">{{ line.text }}</span>
                        <span v-if="line.status" :class="[line.statusColor || 'text-white', 'warming-glow']">
                            {{ line.status }}
                        </span>
                    </div>
                </template>
                <!-- Blinking Cursor -->
                <div v-if="lines.length < sequence.length" class="h-5 w-2 bg-indigo-500 animate-pulse mt-1"></div>
            </div>

            <div class="mt-8 relative h-1 w-full bg-white/5 rounded-full overflow-hidden">
                <div
                    class="absolute inset-y-0 left-0 bg-linear-to-r from-indigo-500 to-purple-600 animate-[loading_2.5s_ease-in-out_forwards] rounded-full w-full">
                </div>
            </div>
        </div>
    </div>
</template>

<style scoped>
.animate-fade-in {
    animation: fadeIn 0.3s ease-out forwards;
}

@keyframes fadeIn {
    from {
        opacity: 0;
        transform: translateY(5px);
    }

    to {
        opacity: 1;
        transform: translateY(0);
    }
}

@keyframes loading {
    0% {
        width: 0%;
        opacity: 1;
    }

    50% {
        width: 40%;
    }

    80% {
        width: 80%;
    }

    100% {
        width: 100%;
        opacity: 0;
    }
}

.warming-glow {
    text-shadow: 0 0 10px rgba(99, 102, 241, 0.5);
}
</style>