<script setup lang="ts">
import { onMounted, ref } from 'vue';

const props = defineProps<{
    connectionStatus: 'pending' | 'success' | 'error';
}>();

const emit = defineEmits<{
    (e: 'complete'): void;
}>();

const lines = ref<{ text: string, status?: string, statusColor?: string }[]>([]);
const isComplete = ref(false);

const initialSequence = [
    { text: "> INITIALIZING V.O.I.D. KERNEL...", delay: 100 },
    { text: "> LOADING MEMORY MODULES...", status: "OK", statusColor: "text-indigo-400", delay: 300 },
    { text: "> MOUNTING FILE SYSTEM...", status: "OK", statusColor: "text-indigo-400", delay: 300 },
    { text: "> CHECKING INTEGRITY...", status: "VERIFIED", statusColor: "text-emerald-400 font-bold", delay: 400 },
];

const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?/";

const typeText = async (text: string, onUpdate: (txt: string) => void) => {
    let current = "";
    for (let i = 0; i < text.length; i++) {
        current += text[i];
        onUpdate(current);
        await new Promise(r => setTimeout(r, 15 + Math.random() * 20)); // Faster typing
    }
};

const decodeStatus = async (targetStatus: string, onUpdate: (txt: string) => void) => {
    const duration = 500;
    const steps = 10;
    const stepTime = duration / steps;

    for (let i = 0; i < steps; i++) {
        let scrambled = "";
        for (let j = 0; j < targetStatus.length; j++) {
            scrambled += chars[Math.floor(Math.random() * chars.length)];
        }
        onUpdate(scrambled);
        await new Promise(r => setTimeout(r, stepTime));
    }
    onUpdate(targetStatus);
};

onMounted(async () => {
    // 1. Run Initial Sequence
    for (const item of initialSequence) {
        const lineIdx = lines.value.push({ text: "", status: "" }) - 1;

        await typeText(item.text, (txt) => {
            if (lines.value[lineIdx]) {
                lines.value[lineIdx].text = txt;
            }
        });

        if (item.status) {
            await new Promise(r => setTimeout(r, 100));
            await decodeStatus(item.status, (txt) => {
                if (lines.value[lineIdx]) {
                    lines.value[lineIdx].status = txt;
                    if (txt === item.status) lines.value[lineIdx].statusColor = item.statusColor;
                }
            });
        }
        await new Promise(r => setTimeout(r, item.delay));
    }

    // 2. Establish Connection Phase
    const connLineIdx = lines.value.push({ text: "> ESTABLISHING CONNECTION...", status: "WAITING" }) - 1;

    // Wait until status changes from pending
    while (props.connectionStatus === 'pending') {
        await new Promise(r => setTimeout(r, 100));
    }

    // 3. Handle Result
    if (props.connectionStatus === 'success') {
        const finalText = "> CONNECTION ESTABLISHED";
        await typeText(finalText.replace("> ", ""), (txt) => {
            if (lines.value[connLineIdx]) {
                lines.value[connLineIdx].text = "> " + txt;
            }
        });
        // Just update status
        await decodeStatus("ONLINE", (txt) => {
            if (lines.value[connLineIdx]) {
                lines.value[connLineIdx].status = txt;
                if (txt === "ONLINE") lines.value[connLineIdx].statusColor = "text-purple-400 font-bold glow-text";
            }
        });

        await new Promise(r => setTimeout(r, 500));
        isComplete.value = true;

        // Wait for progress bar fill animation
        await new Promise(r => setTimeout(r, 600));
        emit('complete');
    } else {
        await decodeStatus("OFFLINE", (txt) => {
            if (lines.value[connLineIdx]) {
                lines.value[connLineIdx].status = txt;
                if (txt === "OFFLINE") lines.value[connLineIdx].statusColor = "text-red-500 font-bold";
            }
        });
        // Stop here.
    }
});
</script>

<template>
    <div class="flex flex-col items-center justify-center w-full h-full min-h-[300px] select-none">
        <div class="font-mono text-sm tracking-tight p-8 min-w-[320px] w-full max-w-xl">
            <div class="flex flex-col gap-2">
                <template v-for="(line, index) in lines" :key="index">
                    <div class="flex justify-between items-end gap-4 border-b border-white/5 pb-1">
                        <span class="text-gray-400 font-medium">{{ line.text }}</span>
                        <span v-if="line.status"
                            :class="[line.statusColor || 'text-gray-500', 'text-xs uppercase tracking-widest']">
                            [{{ line.status }}]
                        </span>
                    </div>
                </template>
            </div>

            <!-- Progress Bar -->
            <div class="mt-8 h-1 w-full bg-gray-800/50 rounded-full overflow-hidden">
                <div class="h-full bg-indigo-500 shadow-[0_0_10px_rgba(99,102,241,0.5)] transition-all duration-700 ease-out"
                    :style="{ width: isComplete ? '100%' : `${(lines.length / (initialSequence.length + 1)) * 90}%` }">
                </div>
            </div>

            <div class="flex justify-between mt-2 text-[10px] text-gray-500 uppercase tracking-widest">
                <span>System_Boot_Seq_v2.0</span>
                <span>{{ isComplete ? '100%' : Math.floor((lines.length / (initialSequence.length + 1)) * 90) + '%'
                    }}</span>
            </div>
        </div>
    </div>
</template>

<style scoped>
.glow-text {
    text-shadow: 0 0 10px rgba(168, 85, 247, 0.6);
}
</style>