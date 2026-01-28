#pragma once
#include "./mapper/mapper.h"
#include "../env.h"
#include "../utils/javautils.h"
#include "../../dll/debug.h"

namespace Java {
	class CMinecraft {
	private:
		CMinecraft() = default;
        ~CMinecraft() { Cleanup(); };

		jobject internal_mc_instance = nullptr;
	public:
        static CMinecraft& Get() {
            static CMinecraft instance;
            return instance;
        }

        CMinecraft(const CMinecraft&) = delete;
        void operator=(const CMinecraft&) = delete;

        bool Initialize();

        jobject GetJObject();
        jobject GetFieldObject(std::string fieldKey);

        void Cleanup();
	};
}