"use client";

import { Box, BugOff, Cloud, Shredder, Sparkles, Swords } from "lucide-react";
import CardInformation from "@/shared/components/common/CardInformation";
import CardProduct from "@/modules/home/components/client/ui/CardProduct";
import HeroFeature from "@/modules/home/components/client/ui/HeroFeature";

export default function Home() {
  const products = [
    {
      title: "Void: Cloud Combat",
      subtitle: "Complete combat suite",
      price: "$15.00",
      period: "/month",
      features: [
        "Access to legit modules",
        "Trace Removal Protocol",
        "Priority 24/7 Support",
        "Early Access to Updates",
        "Windows 10/11 Compatible"
      ],
      isPopular: true,
      view: "Void: Cloud Combat"
    }
  ];

  return (
    <>
      <div className="flex flex-col gap-12 pb-10">
        <section>
          <HeroFeature />
        </section>

        <section className="flex flex-col gap-6">
          <div className="flex items-center gap-3 mb-2">
            <Sparkles className="w-5 h-5 text-indigo-400" />
            <h2 className="text-xl font-bold text-white tracking-wide">Why choose us?</h2>
            <div className="h-px bg-white/10 flex-1"></div>
          </div>

          <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">
            <CardInformation>
              <div className="flex items-start gap-5">
                <div
                  className="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                  <Swords className="h-6 w-6 text-indigo-400" />
                </div>
                <div>
                  <h3 className="text-lg font-bold text-white mb-1">Humanized Combat Logic</h3>
                  <p className="text-sm leading-relaxed text-gray-400">
                    Timeless architecture. Universal modules built to outlast any anticheat update.
                  </p>
                </div>
              </div>
            </CardInformation>

            <CardInformation>
              <div className="flex items-start gap-5">
                <div
                  className="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                  <Cloud className="h-6 w-6 text-indigo-400" />
                </div>
                <div>
                  <h3 className="text-lg font-bold text-white mb-1">Performance</h3>
                  <p className="text-sm leading-relaxed text-gray-400">
                    Native C++ performance controlled from the cloud. Decoupled interface for a seamless
                    experience.
                  </p>
                </div>
              </div>
            </CardInformation>

            <CardInformation>
              <div className="flex items-start gap-5">
                <div
                  className="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                  <Shredder className="h-6 w-6 text-indigo-400" />
                </div>
                <div>
                  <h3 className="text-lg font-bold text-white mb-1">Trace Removal Protocol</h3>
                  <p className="text-sm leading-relaxed text-gray-400 max-w-3xl">
                    Instant memory wipe. Cleans strings and prefetch data to leave zero forensic
                    evidence on the target machine.
                  </p>
                </div>
              </div>
            </CardInformation>

            <CardInformation>
              <div className="flex items-start gap-5">
                <div
                  className="flex h-12 w-12 shrink-0 items-center justify-center rounded-xl bg-indigo-500/10 ring-1 ring-inset ring-indigo-500/20 shadow-[0_0_15px_rgba(99,102,241,0.15)]">
                  <BugOff className="h-6 w-6 text-indigo-400" />
                </div>
                <div>
                  <h3 className="text-lg font-bold text-white mb-1">Active support</h3>
                  <p className="text-sm leading-relaxed text-gray-400 max-w-3xl">
                    Rest easy knowing our dedicated team has your back. We are here for you 24/7 to
                    ensure you absolutely love your purchase.
                  </p>
                </div>
              </div>
            </CardInformation>
          </div>
        </section>

        <section className="flex flex-col gap-6">
          <div className="flex items-center gap-3 mb-2">
            <Box className="w-5 h-5 text-indigo-400" />
            <h2 className="text-xl font-bold text-white tracking-wide">Available Products</h2>
            <div className="h-px bg-white/10 flex-1"></div>
          </div>

          <div className="grid grid-cols-1 lg:grid-cols-2 gap-6" id="prod_str">
            {products.map((product, index) => (
              <CardProduct
                key={index}
                title={product.title}
                subtitle={product.subtitle}
                price={product.price}
                period={product.period}
                features={product.features}
                isPopular={product.isPopular}
                onBuy={() => { }}
              />
            ))}
          </div>
        </section>
      </div>
    </>
  );
}
