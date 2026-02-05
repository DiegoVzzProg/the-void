#pragma once

//#include "../utils/math.h"
#include "../minecraft/minecraft.h"
#include "../env.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vector3 { float x, y, z; };
struct Vector2 { float yaw, pitch; };

namespace Java {

	class CFunctions {
	private:

		CFunctions() = default;

		jobject GetMouseOver();
		jobject GetCurrentScreen();

		bool McIsValid();
	public:

		static CFunctions& Get() {
			static CFunctions instance;
			return instance;
		}

		CFunctions(const CFunctions&) = delete;
		void operator=(const CFunctions&) = delete;

		jobject GetWorld();
		jobject GetPlayer();

		std::vector<jobject> GetWorldPlayers();
		Vector3 GetEntityPos(jobject entity);
		Vector3 GetEyePos(jobject entity);
		Vector2 GetMyRotation();
		void SetMyRotation(Vector2 rots);

		bool InGui();
		bool IsInInventory();

		bool IsHoldingSword();
		bool IsHoldingWeapon();
		bool TargetHasDamage();
		bool IsLookingAtBlock();
		bool IsLookingAtEntity();
		bool IsIngame();
		void SetRightClickState(bool pressed);
		float GetHeight(jobject entity);
		float GetMouseSensitivity();
		int GetHurtTime(jobject entity);
		float GetSwingProgress(jobject entity);
	};
}