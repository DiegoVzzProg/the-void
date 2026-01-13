#pragma once

struct FirstHit {
	inline static bool Enabled = false;
	inline static float Range = 0.00f;
	
	static void Update();

	inline static float currentOffset = 0.0f; // Dónde estamos ahora
	inline static float targetOffset = 0.0f;  // Hacia dónde vamos
	inline static float velocity = 0.0f;      // Qué tan rápido cambia el ping
	inline static long long lastUpdate = 0;
};