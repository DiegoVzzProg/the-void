#pragma once
#include "../../../util/math/geometry.h"

#include <string>
#include <vector>

struct Reach
{
	inline static bool Enabled = false;

	inline static float MinReach = 3.05f;
	inline static float MaxReach = 3.20f;
	inline static float hitChance = 100.0f;
	inline static bool Randomization = false;

	static void Update();
	static void RenderMenu();
};

