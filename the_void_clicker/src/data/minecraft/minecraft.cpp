#include "./mapper/mapper.h"
#include "../env.h"
#include "../utils/javautils.h"
#include "../../dll/debug.h"

#include "minecraft.h"

namespace Java {
	jobject CMinecraft::GetInstance() {
		return instance;
	}

	CMinecraft CMinecraft::GetMinecraft() {
		if (!Java::CEnv::env) return CMinecraft(nullptr);

		struct Candidate {
			std::string className;
			std::string methodName;
			std::string signature;
			bool isMethod;
		};

		std::vector<Candidate> attempts = {
			{ "net/minecraft/client/Minecraft", "getMinecraft", "()Lnet/minecraft/client/Minecraft;", true },
			{ "ave", "A", "()Lave;", true },
			{ "ave", "S", "Lave;", false },
			{ "ave", "theMinecraft", "Lave;", false },
			{ "net/minecraft/client/Minecraft", "theMinecraft", "Lnet/minecraft/client/Minecraft;", false }
		};

		for (const auto& attempt : attempts) {
			// 1. Buscamos la clase (Usando tu Utils::FindClass que arregla el ClassLoader)
			jclass cls = Utils::FindClass(attempt.className.c_str());

			if (!cls) {
				Java::CEnv::env->ExceptionClear();
				continue; // Si no existe la clase, probamos el siguiente
			}

			jobject mcObj = nullptr;

			if (attempt.isMethod) {
				jmethodID mid = Java::CEnv::env->GetStaticMethodID(cls, attempt.methodName.c_str(), attempt.signature.c_str());
				if (mid) {
					mcObj = Java::CEnv::env->CallStaticObjectMethod(cls, mid);
					static bool msg1 = false;
					if (!msg1) {
						msg1 = true;
						DEBUG_LOG("[THE VOID] Instancia encontrada via METODO: " << attempt.className << "::" << attempt.methodName);
					}
				}
			}
			else {
				jfieldID fid = Java::CEnv::env->GetStaticFieldID(cls, attempt.methodName.c_str(), attempt.signature.c_str());
				if (fid) {
					mcObj = Java::CEnv::env->GetStaticObjectField(cls, fid);
					static bool msg2 = false;
					if (!msg2) {
						msg2 = true;
						DEBUG_LOG("[THE VOID] Instancia encontrada via CAMPO: " << attempt.className << "::" << attempt.methodName);
					}
				}
			}

			if (!mcObj) Java::CEnv::env->ExceptionClear();

			if (mcObj) {
				Java::Mapper::classes[xorstr("Minecraft")] = attempt.className;

				Java::CEnv::env->DeleteLocalRef(cls);
				return CMinecraft(mcObj);
			}

			Java::CEnv::env->DeleteLocalRef(cls);
		}

		static bool msg3 = false;
		if (!msg3) {
			msg3 = true;
			DEBUG_LOG("[FATAL] No se pudo obtener la instancia de Minecraft en ningun intento.");
		}
		return CMinecraft(nullptr);
	}
}