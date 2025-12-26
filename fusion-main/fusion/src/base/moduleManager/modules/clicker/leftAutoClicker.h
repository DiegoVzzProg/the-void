#pragma once
#include "../../commonData.h"

struct LeftAutoClicker
{
	inline static bool Enabled = false;

	inline static float leftMaxCps = 14;
	inline static float leftMinCps = 10;
	inline static bool ignoreBlocks = true;
	inline static bool autoBlockHit = false;
	inline static float blockhitchance = 10;

	static void Update();
	static void RenderMenu();

	static CommonData::PlayerData GetBestTarget(float fov, float range); 
};

