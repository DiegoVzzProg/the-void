import { create } from 'zustand';
import { IAuthState } from '@/modules/auth';

export const useUserStore = create<IAuthState>((set) => ({
    user: null,
    isAuthenticated: false,
    setUser: (user) => set({ user, isAuthenticated: !!user }),
}));