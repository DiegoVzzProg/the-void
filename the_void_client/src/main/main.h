#pragma once
#include <windows.h>
#include <iostream>
#include <thread>

#include "../utils/xorstr.h"
#include "../java/env.h"
#include "../java/mapper/Mapper.h"
#include "../java/sdk/Minecraft.h"

#include "../modules/modules.h"

namespace TheVoid {
	namespace Main {
		class CMain {
		protected:
            static void Test() {
                auto mc = TheVoid::SDK::Minecraft::GetMinecraft();

                if (mc.IsValid()) {
                    auto player = mc.GetThePlayer();

                    if (player.IsValid()) {
                        float health = player.GetHealth();

                        // Si llegamos aquí, todo funciona
                        std::cout << "\r[THE VOID] Vida: " << health << " | Jugador OK   " << std::flush;

                        TheVoid::Java::env->DeleteLocalRef(player.GetInstance());
                    }
                    else {
                        // Debug para saber si falla al encontrar al jugador
                        std::cout << "\r[THE VOID] Minecraft OK | Jugador: NULL (Menu?) " << std::flush;
                    }

                    TheVoid::Java::env->DeleteLocalRef(mc.GetInstance());
                }
                else {
                    // Debug para saber si falla al encontrar Minecraft
                    std::cout << "\r[THE VOID] Buscando clase Minecraft... " << std::flush;
                }
            };
		public:
			static void Init(HMODULE hModule);
		};
	}
}