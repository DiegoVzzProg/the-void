#pragma once
#include <chrono>

#include "../../commonData.h"
#include "../../../sdk/sdk.h"

struct LeftAutoClicker
{
	inline static bool Enabled = false;

	inline static float leftMaxCps = 14;
	inline static float leftMinCps = 10;
	inline static bool ignoreBlocks = true;

	inline static bool autoBlockHit = false;

	inline static std::chrono::steady_clock::time_point lastBlockTime = std::chrono::steady_clock::now();
	inline static int nextBlockDelay = 1000;
	inline static bool inventory = false;

	static void Update();
	static void RenderMenu();

	static CommonData::PlayerData GetBestTarget(float fov, float range); 
};

struct AutoBlockhit {
	inline static std::atomic<bool> Enabled = false;

	inline static std::atomic<int> MinDelay = 900;
	inline static std::atomic<int> MaxDelay = 1100;

	inline static jclass KeyBindingClass = nullptr;
	inline static jfieldID pressedFieldID = nullptr;
	inline static jobject rightClickKeyObj = nullptr; 
	inline static jclass ItemSwordClass = nullptr;

	inline static std::atomic<bool> isBlockingInternal = false;

	static void InitializeBlockHitCache();
	static void Update();
	static bool IsHoldingSword();
};