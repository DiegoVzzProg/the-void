#pragma once
#include "../../commonData.h"
#include <chrono>

struct LeftAutoClicker
{
	inline static bool Enabled = false;

	inline static float leftMaxCps = 14;
	inline static float leftMinCps = 10;
	inline static bool ignoreBlocks = true;
	inline static bool autoBlockHit = false;
	inline static float blockhitchance = 10;
	inline static std::chrono::steady_clock::time_point lastBlockTime = std::chrono::steady_clock::now();
	inline static int nextBlockDelay = 1000;

	static void Update();
	static void RenderMenu();
	static void AutoBlockHit();

	static CommonData::PlayerData GetBestTarget(float fov, float range); 
};

