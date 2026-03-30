"use client";

import { ShoppingBag, Check, Zap, Star, Clock } from "lucide-react";

interface ProductProps {
    title: string;
    subtitle?: string;
    price: string;
    period?: string;
    features?: string[];
    isPopular?: boolean;
    isComingSoon?: boolean;
    onBuy?: () => void;
}

export default function CardProduct({ title, subtitle, price, period, features, isPopular, isComingSoon, onBuy }: ProductProps) {
    return (
        <div className="group relative flex flex-col w-full h-full transition-all duration-500 ease-out hover:-translate-y-2">
            <style>{`
                @keyframes shimmer {
                    100% {
                        transform: translateX(100%);
                    }
                }
            `}</style>
            
            <div
                className={`relative flex flex-col h-full rounded-4xl overflow-hidden backdrop-blur-xl border flex-1 transition-all duration-500 ease-out ${
                    isPopular
                        ? "bg-[#0b0c15]/80 border-indigo-500/10 shadow-2xl shadow-indigo-500/5"
                        : "bg-[#0e1016]/40 border-white/5 hover:bg-[#0e1016]/60 hover:border-white/10"
                }`}
            >
                {/* Popular Glow */}
                {isPopular && (
                    <div className="absolute inset-0 rounded-4xl border border-indigo-500/10 pointer-events-none z-10 transition-colors duration-500"></div>
                )}

                <div className="p-6 md:p-8 flex flex-col gap-4 border-b border-white/5 relative bg-white/1 transition-colors duration-500">
                    {/* Badges Container */}
                    <div className="absolute top-6 right-6 flex flex-col items-end gap-2">
                        {isPopular && (
                            <div className="inline-flex items-center gap-1.5 rounded-full bg-indigo-500/10 border border-indigo-500/20 px-3 py-1 text-[10px] font-bold uppercase tracking-widest text-indigo-300/80 shadow-[0_0_15px_rgba(99,102,241,0.15)] transition-all duration-500 group-hover:bg-indigo-500/20">
                                <Star className="w-3 h-3 fill-indigo-300/80 text-indigo-300" />
                                Popular
                            </div>
                        )}
                        {isComingSoon && (
                            <div className="inline-flex items-center gap-1.5 rounded-full bg-amber-500/5 border border-amber-500/10 px-3 py-1 text-[10px] font-bold uppercase tracking-widest text-amber-300/90 shadow-[0_0_15px_rgba(245,158,11,0.1)] transition-all duration-500 group-hover:bg-amber-500/10">
                                <Clock className="w-3 h-3 text-amber-300/90" />
                                Coming Soon
                            </div>
                        )}
                    </div>

                    <div>
                        <h3 className="text-lg md:text-xl font-bold text-white/90 tracking-tight flex items-center gap-2 group-hover:text-white transition-colors duration-500">
                            {title}
                            {isPopular && <Zap className="w-4 h-4 text-yellow-400/80 fill-yellow-400/80" />}
                        </h3>
                        <p className="text-sm text-gray-400 mt-1 font-medium transition-colors duration-500 group-hover:text-gray-300">
                            {subtitle}
                        </p>
                    </div>

                    <div className="flex items-end gap-1 mt-2">
                        <span
                            className={`text-3xl md:text-4xl font-bold text-white/90 tracking-tight leading-none transition-colors duration-500 group-hover:text-white ${
                                isComingSoon ? "text-white/40" : ""
                            }`}
                        >
                            {price}
                        </span>
                        {period && (
                            <span className="text-sm font-medium text-gray-500 mb-1.5 group-hover:text-gray-400 transition-colors duration-500">
                                {period}
                            </span>
                        )}
                    </div>
                </div>

                <div className="p-6 md:p-8 flex flex-col flex-1">
                    <ul className="space-y-4 mb-8 flex-1">
                        {features?.map((feature, index) => (
                            <li key={index} className="flex items-start gap-3 group/item">
                                <div
                                    className={`mt-0.5 flex h-5 w-5 shrink-0 items-center justify-center rounded-full transition-all duration-500 ease-out ${
                                        isPopular
                                            ? "bg-indigo-500/10 text-indigo-400/80 group-hover/item:bg-indigo-500/20 group-hover/item:text-indigo-300"
                                            : "bg-white/5 text-gray-500 group-hover/item:bg-white/10 group-hover/item:text-gray-300"
                                    } ${isComingSoon && !isPopular ? "grayscale opacity-40" : ""}`}
                                >
                                    <Check className="h-3 w-3" strokeWidth={4} />
                                </div>
                                <span
                                    className={`text-sm text-gray-400 font-medium leading-relaxed transition-colors duration-500 group-hover/item:text-gray-300 ${
                                        isComingSoon ? "text-gray-600" : ""
                                    }`}
                                >
                                    {feature}
                                </span>
                            </li>
                        ))}
                    </ul>

                    <button
                        onClick={() => !isComingSoon && onBuy && onBuy()}
                        disabled={isComingSoon}
                        className={`w-full relative overflow-hidden rounded-xl py-4 text-sm font-bold transition-all duration-500 ease-out active:scale-[0.98] mt-auto ${
                            isComingSoon
                                ? "bg-amber-500/5 text-amber-500/70 border border-amber-500/10 select-none"
                                : isPopular
                                ? "bg-indigo-600 text-white shadow-lg shadow-indigo-500/20 hover:bg-indigo-500 hover:shadow-indigo-500/30"
                                : "bg-white/5 text-white/90 border border-white/5 hover:bg-white/10 hover:border-white/10"
                        }`}
                    >
                        {isPopular && !isComingSoon && (
                            <div className="absolute inset-0 -translate-x-full group-hover:animate-[shimmer_2s_infinite] bg-linear-to-r from-transparent via-white/10 to-transparent z-0"></div>
                        )}

                        <span className="relative z-10 flex items-center justify-center gap-2">
                            {isComingSoon ? (
                                <>
                                    Coming Soon
                                    <Clock className="w-4 h-4" />
                                </>
                            ) : (
                                <>
                                    Get Started
                                    <ShoppingBag className="w-4 h-4 transition-transform group-hover:translate-x-1" />
                                </>
                            )}
                        </span>
                    </button>
                </div>
            </div>
        </div>
    );
}