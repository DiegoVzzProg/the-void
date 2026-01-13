<script setup lang="ts">
import { LockKeyhole, Mail, User, UserPlus } from 'lucide-vue-next';
import type { IRegister } from '../../interfaces/auth.interface';
import { AuthService } from '../../services/auth.service';
import { useAuthStorage } from '../../stores/auth.store';
import { useUserStorage } from '../../stores/user.store';
import { Utils } from '../../utilities/utilities';

const voidTheme = {
    outer: 'mb-5 group w-full',
    label: 'block mb-2 text-xs font-medium text-zinc-400 group-focus-within:text-indigo-400 transition-colors', // Label cambia de color al enfocar
    inner: 'flex items-center bg-[#151519] border border-white/10 rounded-xl focus-within:border-indigo-500/50 focus-within:ring-2 focus-within:ring-indigo-500/20 transition-all overflow-hidden',
    input: 'w-full bg-transparent text-sm text-white placeholder-zinc-600 px-3 py-3 focus:outline-none autofill:bg-[#151519]', // Input transparente dentro del contenedor
    help: 'text-xs text-zinc-600 mt-1',
    message: 'text-red-400 text-xs mt-1.5'
}

const handleRegister = async (formData: any) => {
    try {

        const form: IRegister = {
            email: formData.email,
            username: formData.username,
            password: formData.password
        }

        const res = await new AuthService().Register(form);

        if (!res.data) {
            return;
        }

        const data = res.data;
        useAuthStorage()
            .saveToken("__voidAccessToken", data.accessToken, 60 * 15);
        useAuthStorage()
            .saveToken("__voidRefreshToken", data.refreshToken, 60 * 60 * 24 * 7);

        useAuthStorage().deactivateOption("yes");

        useUserStorage().set(data.user.username, data.user.email);

        const site = new Utils();
        site.RedirectPage("information");
    }
    catch (error: any) {
        console.error(error);
    }
}

const seeLoginForm = () => {
    const site = new Utils();
    site.RedirectPage({ name: "auth", params: { view: "login" } })
}
</script>

<template>
    <div
        class="relative w-full max-w-[620px] bg-[#0e0e11] border border-white/10 rounded-2xl p-8 shadow-2xl shadow-black/50 backdrop-blur-sm">

        <div class="mb-8 text-center">
            <div
                class="w-12 h-12 bg-linear-to-br from-indigo-500 to-violet-600 rounded-xl mx-auto mb-4 flex items-center justify-center shadow-lg shadow-indigo-500/20">
                <UserPlus class="size-6" />
            </div>
            <h1 class="text-2xl font-bold text-white tracking-tight">Join The Void</h1>
            <p class="text-zinc-500 text-sm mt-2">Create your stealth account to proceed.</p>
        </div>

        <FormKit type="form" submit-label="Get Started" @submit="handleRegister" :incomplete-message="false"
            :submit-attrs="{
                inputClass: 'w-full bg-gradient-to-r from-indigo-600 to-violet-600 hover:from-indigo-500 hover:to-violet-500 text-white font-medium py-3 rounded-xl transition-all shadow-lg shadow-indigo-500/25 mt-2'
            }">
            <div class="flex flex-col md:flex-row gap-2">
                <FormKit type="text" name="username" label="Username" placeholder="GhostPilot"
                    validation="required|length:3,16|matches:/^[a-zA-Z0-9_]+$/" :classes="voidTheme">
                    <template #prefix>
                        <div class="pl-3 text-zinc-500">
                            <User class="size-5" />
                        </div>
                    </template>
                </FormKit>

                <FormKit type="email" name="email" label="Email Address" placeholder="you@example.com"
                    validation="required|email" :classes="voidTheme">
                    <template #prefix>
                        <div class="pl-3 text-zinc-500">
                            <Mail class="size-5" />
                        </div>
                    </template>
                </FormKit>
            </div>

            <div class="flex flex-col md:flex-row gap-2">
                <FormKit type="password" name="password" label="Password" placeholder="••••••••"
                    validation="required|length:8" :classes="voidTheme">
                    <template #prefix>
                        <div class="pl-3 text-zinc-500">
                            <LockKeyhole class="size-5" />
                        </div>
                    </template>
                </FormKit>

                <FormKit type="password" name="password_confirm" label="Confirm Password" placeholder="••••••••"
                    validation="required|confirm" validation-label="Password confirmation" :classes="voidTheme">
                    <template #prefix>
                        <div class="pl-3 text-zinc-500">
                            <LockKeyhole class="size-5" />
                        </div>
                    </template>
                </FormKit>
            </div>

        </FormKit>

        <div class="mt-8 pt-6 border-t border-white/5 text-center">
            <p class="text-zinc-500 text-sm">
                Already have an account?
                <button v-on:click="seeLoginForm"
                    class="text-indigo-400 hover:text-indigo-300 font-medium transition-colors">
                    Sign In
                </button>
            </p>
        </div>

    </div>
</template>

<style scoped></style>