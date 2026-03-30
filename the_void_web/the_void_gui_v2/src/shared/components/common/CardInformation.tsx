"use client";

interface CardInformationProps {
    children: React.ReactNode;
}

export default function CardInformation({ children }: CardInformationProps) {
    return (
        <div className="group relative flex flex-col items-start gap-4 p-6 rounded-2xl border border-white/5 bg-[#13151f]/50 backdrop-blur-sm transition-all duration-300 ease-out hover:border-indigo-500/30 hover:bg-[#13151f] hover:-translate-y-1 hover:shadow-xl hover:shadow-indigo-500/10">
            <div className="absolute inset-0 rounded-2xl bg-linear-to-br from-indigo-500/5 to-transparent opacity-0 transition-opacity group-hover:opacity-100 pointer-events-none"></div>

            <div className="relative z-10 w-full">
                {children}
            </div>
        </div>
    );
}