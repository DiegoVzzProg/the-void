"use client";

import { useEffect, useRef } from "react";
import { useUserStore } from "@/modules/users";

export default function UserInitializer({ user }: { user: any | null }) {
    const initialized = useRef(false);
    const setUser = useUserStore((state) => state.setUser);

    if (!initialized.current) {
        useUserStore.setState({ user, isAuthenticated: !!user });
        initialized.current = true;
    }

    useEffect(() => {
        setUser(user);
    }, [user, setUser]);

    return null;
}