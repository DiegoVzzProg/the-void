import "./globals.css";
import type { Metadata } from "next";
import { redirect } from "next/navigation";
import { fetchSession } from "@/modules/users";
import Sidebar from "@/modules/home/components/system/Sidebar";

export const metadata: Metadata = {
  title: "Void - Ghost Client",
  description: "",
};

export default async function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  const user = await fetchSession();

  if (!user || user.role !== "ADMIN") {
    return redirect("/");
  }

  return (
    <html lang="en">
      <body>
        <div className="flex flex-row w-full h-dvh overflow-hidden relative z-10 text-white">
          <Sidebar />
          <main className="flex flex-col flex-1 overflow-hidden relative">
            {children}
          </main>
        </div>
      </body>
    </html>
  );
}
