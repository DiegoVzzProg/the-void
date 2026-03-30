"use client";

import { User, Cpu, Shield, Trash2, Calendar, Crown, ArrowRight, ShieldAlert, AlertTriangle, DoorOpen } from "lucide-react";
import { useRouter } from "next/navigation";
import { useUserStore } from "@/modules/users";
import CardInformation from "@/shared/components/common/CardInformation";
import { DeleteCookieServer } from "@/modules/auth";
import { Site, TypeToast } from "@/shared/utils/site.util";

export default function UserPage() {
    const user = useUserStore(state => state.user);
    const router = useRouter();

    const handleAction = (action: string) => {

        Site.Toast({
            type: TypeToast.INFO,
            title: action,
            message: "Functionality coming soon."
        });
    };

    const logoutAction = async () => {
        await DeleteCookieServer();
        useUserStore.getState().setUser(null);
        Site.Toast({
            type: TypeToast.SUCCESS,
            title: "Session closed successfully",
            message: "You have been logged out of your account."
        });
        router.push('/auth');
    }

    return (
        <div className="flex flex-col gap-6 w-full mx-auto pb-10">
            {/* Header */}
            <div className="flex items-center gap-4 mb-2">
                <div className="size-16 rounded-2xl bg-indigo-500/10 border border-indigo-500/20 flex items-center justify-center shrink-0 shadow-[0_0_15px_-5px_#6366f1]">
                    <User className="size-8 text-indigo-400" />
                </div>
                <div>
                    <h1 className="text-2xl font-bold text-white tracking-tight flex items-center gap-2">
                        {user?.username || "Ghost"}
                        {user?.role === "ADMIN" && (
                            <span className="bg-indigo-500/20 text-indigo-400 text-[10px] font-bold px-2 py-0.5 rounded-full border border-indigo-500/30 uppercase tracking-wider flex items-center gap-1">
                                <Crown className="size-3" /> Admin
                            </span>
                        )}
                    </h1>
                    <p className="text-zinc-400 text-sm mt-0.5">{user?.email || "Unknown.Entity"}</p>
                </div>
            </div>

            <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">

                {/* Column 1 */}
                <div className="flex flex-col gap-4">
                    {/* Subscription info */}
                    <CardInformation>
                        <div className="flex items-start gap-4">
                            <div className="flex size-10 items-center justify-center shrink-0 rounded-xl bg-violet-500/10 ring-1 ring-inset ring-violet-500/20">
                                <Calendar className="size-5 text-violet-400" />
                            </div>
                            <div className="flex-1 w-full">
                                <h3 className="text-lg font-bold text-white mb-1">Subscription</h3>
                                <p className="text-sm text-zinc-400 mb-4">Your current access passes and validity.</p>

                                <div className="bg-[#151519] border border-white/5 rounded-xl p-4 flex justify-between items-center mb-1">
                                    <div>
                                        <p className="text-[10px] text-zinc-500 uppercase tracking-wider font-semibold">Current Plan</p>
                                        <p className="text-indigo-400 font-bold text-sm mt-0.5">Lifetime Access</p>
                                    </div>
                                    <div className="text-right">
                                        <p className="text-[10px] text-zinc-500 uppercase tracking-wider font-semibold">Expires</p>
                                        <p className="text-white mt-0.5 text-sm">Never</p>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </CardInformation>

                    {/* Security */}
                    <CardInformation>
                        <div className="flex items-start gap-4 h-full w-full">
                            <div className="flex size-10 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20">
                                <Shield className="size-5 text-indigo-400" />
                            </div>
                            <div className="flex-1 w-full relative">
                                <h3 className="text-lg font-bold text-white mb-1">Security Settings</h3>
                                <p className="text-sm text-zinc-400 mb-5">Update your email and password.</p>

                                <div className="flex flex-col gap-4 w-full">
                                    <div className="group relative">
                                        <label className="block mb-1.5 text-xs font-medium text-zinc-400 transition-colors select-none">Email Address</label>
                                        <div className="flex gap-2">
                                            <input type="text" defaultValue={user?.email || ""} className="w-full text-sm text-white placeholder-zinc-600 px-3 py-2.5 rounded-xl focus:outline-none bg-[#151519] border border-white/5 focus:border-indigo-500/50" />
                                            <button onClick={() => handleAction('Update Email')} className="bg-[#151519] hover:bg-white/5 border border-white/5 text-white text-sm px-4 rounded-xl font-medium transition-colors shrink-0">Update</button>
                                        </div>
                                    </div>

                                    <div className="border-t border-white/5 my-1"></div>

                                    <div className="group relative">
                                        <label className="block mb-1.5 text-xs font-medium text-zinc-400 transition-colors select-none">New Password</label>
                                        <div className="flex gap-2">
                                            <input type="password" placeholder="••••••••" className="w-full text-sm text-white placeholder-zinc-600 px-3 py-2.5 rounded-xl focus:outline-none bg-[#151519] border border-white/5 focus:border-indigo-500/50" />
                                            <button onClick={() => handleAction('Change Password')} className="bg-[#151519] hover:bg-white/5 border border-white/5 text-white text-sm px-4 rounded-xl font-medium transition-colors shrink-0">Change</button>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </CardInformation>

                </div>


                {/* Column 2 */}
                <div className="flex flex-col gap-4">
                    {user && (
                        <div className="group relative flex flex-col items-start p-6 rounded-2xl border border-rose-500/20 bg-rose-500/5 backdrop-blur-sm transition-all duration-300 hover:border-rose-500/40 hover:bg-rose-500/10 hover:-translate-y-1 hover:shadow-xl hover:shadow-rose-500/10 cursor-pointer"
                            onClick={() => logoutAction()}>
                            <div className="absolute inset-0 rounded-2xl bg-linear-to-br from-rose-500/10 to-transparent opacity-0 transition-opacity group-hover:opacity-100 pointer-events-none"></div>
                            <div className="relative z-10 w-full flex items-center justify-between">
                                <div className="flex items-center gap-4">
                                    <div className="flex size-10 shrink-0 items-center justify-center rounded-xl bg-rose-500/20 ring-1 ring-inset ring-rose-500/30">
                                        <DoorOpen className="size-5 text-rose-400" />
                                    </div>
                                    <div>
                                        <h3 className="text-lg font-bold text-white mb-0.5">Logout</h3>
                                        <p className="text-xs text-rose-300/70">Log out of your account.</p>
                                    </div>
                                </div>
                                <button className="size-10 rounded-full bg-rose-500 flex items-center justify-center text-white group-hover:scale-110 transition-transform">
                                    <ArrowRight className="size-5" />
                                </button>
                            </div>
                        </div>
                    )}

                    {/* HWID */}
                    <CardInformation>
                        <div className="flex items-start gap-4 w-full">
                            <div className="flex size-10 shrink-0 items-center justify-center rounded-xl bg-emerald-500/10 ring-1 ring-inset ring-emerald-500/20">
                                <Cpu className="size-5 text-emerald-400" />
                            </div>
                            <div className="flex-1 w-full">
                                <h3 className="text-lg font-bold text-white mb-1">Hardware ID (HWID)</h3>
                                <p className="text-sm text-zinc-400 mb-4">Your currently bound machine identifier.</p>

                                <div className="bg-[#151519] border border-white/5 rounded-xl p-3 flex items-center justify-center mb-4 text-center">
                                    <code className="text-emerald-400 text-sm font-mono break-all font-semibold tracking-wider">A8F9-2B4C-9D1E-55X2</code>
                                </div>

                                <button onClick={() => handleAction('Reset HWID')} className="w-full py-2.5 bg-[#151519] hover:bg-white/5 border border-white/5 text-white text-sm rounded-xl font-medium transition-colors flex items-center justify-center gap-2">
                                    Request HWID Reset
                                </button>
                            </div>
                        </div>
                    </CardInformation>

                    {/* Admin Panel Link */}
                    {user?.role === "ADMIN" && (
                        <div className="group relative flex flex-col items-start p-6 rounded-2xl border border-indigo-500/20 bg-indigo-500/5 backdrop-blur-sm transition-all duration-300 hover:border-indigo-500/40 hover:bg-indigo-500/10 hover:-translate-y-1 hover:shadow-xl hover:shadow-indigo-500/10 cursor-pointer"
                            onClick={() => handleAction('Admin Controls')}>
                            <div className="absolute inset-0 rounded-2xl bg-linear-to-br from-indigo-500/10 to-transparent opacity-0 transition-opacity group-hover:opacity-100 pointer-events-none"></div>
                            <div className="relative z-10 w-full flex items-center justify-between">
                                <div className="flex items-center gap-4">
                                    <div className="flex size-10 shrink-0 items-center justify-center rounded-xl bg-indigo-500/20 ring-1 ring-inset ring-indigo-500/30">
                                        <ShieldAlert className="size-5 text-indigo-400" />
                                    </div>
                                    <div>
                                        <h3 className="text-lg font-bold text-white mb-0.5">Admin Controls</h3>
                                        <p className="text-xs text-indigo-300/70">Access system dashboard directly</p>
                                    </div>
                                </div>
                                <button className="size-10 rounded-full bg-indigo-500 flex items-center justify-center text-white shadow-[0_0_15px_-3px_#6366f1] group-hover:scale-110 transition-transform">
                                    <ArrowRight className="size-5" />
                                </button>
                            </div>
                        </div>
                    )}

                    {/* Danger Zone */}
                    <div className="mt-auto">
                        <div className="group relative flex flex-col items-start p-6 rounded-2xl border border-red-500/10 bg-[#0e0e11] backdrop-blur-sm transition-all duration-300 h-full">
                            <div className="relative z-10 w-full">
                                <div className="flex items-center gap-3 mb-4">
                                    <AlertTriangle className="size-5 text-red-500" />
                                    <h3 className="text-lg font-bold text-red-500">Danger Zone</h3>
                                </div>
                                <p className="text-sm text-red-400/80 mb-5">Once you delete your account, there is no going back. Please be certain.</p>

                                <button onClick={() => handleAction('Borrar Cuenta')} className="w-full py-2.5 bg-red-500/10 hover:bg-red-500/20 border border-red-500/20 text-red-500 text-sm rounded-xl font-bold transition-colors flex items-center justify-center gap-2">
                                    <Trash2 className="size-4" />
                                    Delete My Account
                                </button>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    );
}