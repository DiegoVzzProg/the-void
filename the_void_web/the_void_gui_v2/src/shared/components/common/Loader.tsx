interface LoaderProps {
    text?: string;
    className?: string;
}

export default function Loader({ text, className = "py-4" }: LoaderProps) {
    return (
        <div className={`flex flex-col items-center justify-center w-full min-h-[48px] ${className}`}>
            <div className="relative flex items-center justify-center w-6 h-6">
                <div className="absolute inset-0 rounded-full border-[1.5px] border-white/5"></div>

                <div className="absolute inset-0 rounded-full border-[1.5px] border-transparent border-t-indigo-500 border-r-indigo-500/40 animate-spin"></div>
            </div>

            {text && (
                <span className="mt-3 text-[10px] font-medium tracking-[0.25em] text-zinc-500 uppercase select-none animate-pulse">
                    {text}
                </span>
            )}
        </div>
    );
}