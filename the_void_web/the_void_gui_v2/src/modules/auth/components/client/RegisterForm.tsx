"use client";

import { zodResolver } from "@hookform/resolvers/zod";
import { UserPlus } from "lucide-react";
import { AnimatePresence, motion } from "motion/react";
import { useRouter } from "next/navigation";
import { useForm } from "react-hook-form";
import { z } from "zod";
import Loader from "@/shared/components/common/Loader";
import { RegisterServer } from "@/modules/auth";
import { Site, TypeToast } from "@/shared/utils/site.util";

const registerSchema = z.object({
    username: z
        .string()
        .min(1, "Username is required")
        .min(4, "Username must be at least 4 characters long")
        .max(16, "Username must be at most 16 characters long"),
    email: z
        .string()
        .min(1, "Email address is required")
        .max(255, "Email address must be at most 255 characters long")
        .email("Please enter a valid email address"),
    password: z
        .string()
        .min(1, "Password is required")
        .min(8, "Password must be at least 8 characters long")
        .max(24, "Password must be at most 24 characters long"),
});

type RegisterFormValues = z.infer<typeof registerSchema>;

export default function RegisterPage() {
    const router = useRouter();

    const {
        register,
        handleSubmit,
        formState: { errors, isSubmitting },
    } = useForm<RegisterFormValues>({
        resolver: zodResolver(registerSchema),
    })

    const onSubmit = async (form: RegisterFormValues) => {
        const res = await RegisterServer(form);
        const data = res.data;

        if (!res.success) {
            Site.Toast({
                type: TypeToast.ERROR,
                title: "¡Attention!",
                message: res.message
            });
            return;
        }

        if (!data) return;

        Site.Toast({
            type: TypeToast.SUCCESS,
            title: "Session started successfully",
            message: "Welcome to The Void."
        });
        router.push('/');
    }
    return (
        <>
            <div className="mb-8 text-center">
                <div
                    className="w-12 h-12 bg-linear-to-br from-indigo-500 to-violet-600 rounded-xl mx-auto mb-4 flex items-center justify-center shadow-lg shadow-indigo-500/20">
                    <UserPlus className="size-6" />
                </div>
                <h1 className="text-2xl font-bold text-white tracking-tight">Join The Void</h1>
                <p className="text-zinc-500 text-sm mt-2">Create your stealth account to proceed.</p>
            </div>

            <form onSubmit={handleSubmit(onSubmit)} className="flex flex-col gap-6">
                <div className="flex flex-col">
                    <label className='block mb-2 text-xs font-medium text-zinc-400 group-focus-within:text-indigo-400 transition-colors select-none'>
                        Username
                    </label>

                    <input
                        {...register("username")}
                        type="text"
                        placeholder="username"
                        className={`w-full text-sm text-white placeholder-zinc-600 px-3 py-3 transition-all duration-300 focus:outline-none bg-[#151519] ${errors.username ? 'ring ring-red-500' : ''}`}
                    />
                    <AnimatePresence>
                        {errors.username && (
                            <motion.p
                                initial={{ opacity: 0, height: 0, marginTop: 0 }}
                                animate={{ opacity: 1, height: "auto", marginTop: 4 }}
                                exit={{ opacity: 0, height: 0, marginTop: 0 }}
                                className="text-sm text-red-500 overflow-hidden"
                            >
                                {errors.username.message}
                            </motion.p>
                        )}
                    </AnimatePresence>
                </div>
                <div className="flex flex-col sm:flex-row gap-1">
                    <div className="flex flex-col w-full">
                        <label className='block mb-2 text-xs font-medium text-zinc-400 group-focus-within:text-indigo-400 transition-colors select-none'>
                            Email Address
                        </label>

                        <input
                            {...register("email")}
                            type="text"
                            placeholder="xxx@email.com"
                            className={`w-full text-sm text-white placeholder-zinc-600 px-3 py-3 transition-all duration-300 focus:outline-none bg-[#151519] ${errors.email ? 'ring ring-red-500' : ''}`}
                        />
                        <AnimatePresence>
                            {errors.email && (
                                <motion.p
                                    initial={{ opacity: 0, height: 0, marginTop: 0 }}
                                    animate={{ opacity: 1, height: "auto", marginTop: 4 }}
                                    exit={{ opacity: 0, height: 0, marginTop: 0 }}
                                    className="text-sm text-red-500 overflow-hidden"
                                >
                                    {errors.email.message}
                                </motion.p>
                            )}
                        </AnimatePresence>
                    </div>
                    <div className="flex flex-col w-full">
                        <label className='block mb-2 text-xs font-medium text-zinc-400 group-focus-within:text-indigo-400 transition-colors select-none'>
                            Password
                        </label>

                        <input
                            {...register("password")}
                            type="password"
                            placeholder="••••••••"
                            className={`w-full text-sm text-white placeholder-zinc-600 px-3 py-3 transition-all duration-300 focus:outline-none bg-[#151519] ${errors.password ? 'ring ring-red-500' : ''}`}
                        />
                        <AnimatePresence>
                            {errors.password && (
                                <motion.p
                                    initial={{ opacity: 0, height: 0, marginTop: 0 }}
                                    animate={{ opacity: 1, height: "auto", marginTop: 4 }}
                                    exit={{ opacity: 0, height: 0, marginTop: 0 }}
                                    className="text-sm text-red-500 overflow-hidden"
                                >
                                    {errors.password.message}
                                </motion.p>
                            )}
                        </AnimatePresence>
                    </div>
                </div>
                {isSubmitting ? (
                    <div className='flex justify-center w-full'>
                        <Loader />
                    </div>
                ) : (
                    <button
                        type="submit"
                        disabled={isSubmitting}
                        className="w-full bg-linear-to-r from-indigo-600 to-violet-600 hover:from-indigo-500 hover:to-violet-500 text-white font-medium py-3 rounded-xl transition-all shadow-lg shadow-indigo-500/25 mt-2 cursor-pointer">
                        {isSubmitting ? 'logging in' : 'access'}
                    </button>
                )}
            </form>
        </>
    );
}