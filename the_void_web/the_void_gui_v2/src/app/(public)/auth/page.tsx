'use client';

import { useState } from "react";
import LoginForm from "@/modules/auth/components/client/LoginForm";
import RegisterForm from "@/modules/auth/components/client/RegisterForm";

export default function AuthPage() {

    const [option, setOption] = useState<'login' | 'register'>('login');

    return (
        <div className='flex justify-center items-center flex-1'>
            <div className='relative w-full max-w-[620px] bg-[#0e0e11] border border-white/10 rounded-2xl p-8 shadow-2xl shadow-black/50 backdrop-blur-sm'>
                <div className="inline-flex flex-row w-full border-b border-white/10 mb-6">
                    <button
                        onClick={() => setOption('login')}
                        className={`w-full py-2 text-sm font-medium transition-colors ${option === 'login' ? 'text-white' : 'text-zinc-500'}`}
                    >
                        Login
                    </button>
                    <button
                        onClick={() => setOption('register')}
                        className={`w-full py-2 text-sm font-medium transition-colors ${option === 'register' ? 'text-white' : 'text-zinc-500'}`}
                    >
                        Register
                    </button>
                </div>
                {option === 'login' ? (
                    <LoginForm />
                ) : (
                    <RegisterForm />
                )}
            </div>
        </div>
    );
}