#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <winsock2.h>       
#include <windows.h>

#include <random>
#include <string>
#include <functional>
#include "../functions.h"
#include "../../web/web.h"

namespace Left {
	class Clicker {
	private:
		Clicker() = default;

		bool enabled = false;
		bool autoBlockHit = false;
		bool inventory = false;
		bool ignore_blocks = false;
		bool weapon_check = false;

		bool isMining = false;
		bool isBlockingActive = false;
		std::chrono::steady_clock::time_point blockReleaseTime;

		int blockChance = 85;
		int blockDuration = 60;

		std::chrono::steady_clock::time_point lastBlockTime;
		long long nextBlockDelay = 0;

		int maxcps = 1;
		int mincps = 0;

		std::chrono::steady_clock::time_point lastClickTime;
		long long nextDelay = 0;

		bool isExhausted = false;
		std::chrono::steady_clock::time_point nextExhaustTime;
		std::chrono::steady_clock::time_point exhaustRecoveryTime;

		// Helper para reiniciar los timers
		void ResetExhaustTimers();

		std::mt19937& GetGenerator();

		long long GetHumanDelay(double currentTargetCPS);
		long long GetHoldTime();
		long long GetNextBlockDelay();

		bool FoundGame();
	public:
		static Clicker& Get() {
			static Clicker instance;
			return instance;
		}

		Clicker(const Clicker&) = delete;
		void operator=(const Clicker&) = delete;

		void Activate();
		void AutoBlockHit();
	};
}