#pragma once

#include "../minecraft/minecraft.h"
#include "../env.h"


namespace Java {
	class CFunctions {
	private:
		CFunctions() = default;

		jobject GetPlayer();
		jobject GetMouseOver();
		jobject GetWorld();
		jobject GetCurrentScreen();

		bool McIsValid();
	public:
		static CFunctions& Get() {
			static CFunctions instance;
			return instance;
		}

		CFunctions(const CFunctions&) = delete;
		void operator=(const CFunctions&) = delete;

		bool InGui();
		bool IsInInventory();

		bool IsHoldingSword();
		bool IsHoldingWeapon();
		bool TargetHasDamage();
		bool IsLookingAtBlock();
		bool IsLookingAtEntity();
		bool IsIngame();
		void SetRightClickState(bool pressed);
	};
}