import "./globals.css";
import type { Metadata } from "next";
import { Toaster } from 'sonner';
import { fetchSession } from "@/modules/users";

import Sidebar from "@/modules/home/components/client/ui/Sidebar";
import PageTitle from "@/modules/home/components/client/ui/PageTitle";
import UserInitializer from "@/modules/users/components/common/DataUser";

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

  return (
    <html lang="en">
      <body>
        <UserInitializer user={user} />
        <div className="fixed top-[-10%] left-[-10%] w-[500px] h-[500px] 
              bg-[#2dd4bf]/10 rounded-full blur-[120px] 
              pointer-events-none animate-pulse z-0">
        </div>

        <div className="fixed bottom-[-10%] right-[-10%] w-[500px] h-[500px] 
              bg-[#f472b6]/10 rounded-full blur-[120px] 
              pointer-events-none animate-pulse z-0" style={{ animationDelay: "1s" }}>
        </div>
        <main className="flex flex-col w-full h-dvh overflow-hidden relative z-10 text-slate-300">
          <div className="flex-1 flex lg:items-center lg:justify-center lg:p-4 overflow-hidden">
            <div className="w-full max-w-[1200px] bg-[#0a0a0c] h-full lg:max-h-[680px] border border-[#232732] lg:rounded-xl flex max-lg:flex-col-reverse shadow-2xl overflow-hidden">

              <Sidebar serverUser={user} />

              <section className="flex flex-col flex-1 relative overflow-hidden bg-[#0a0a0c]">
                <div
                  className="flex-1 w-full h-full overflow-y-auto overflow-x-hidden p-1 md:p-4 lg:p-8 scroll-smooth z-10 relative">
                  <div className="flex flex-col h-full overflow-y-auto overflow-x-hidden px-2">
                    <PageTitle />
                    {children}
                  </div>
                </div>
              </section>
            </div>
          </div>
        </main>

        <Toaster
          theme="dark"
          position="top-right"
          closeButton
          richColors
          duration={3000}
        />
      </body>
    </html>
  );
}
