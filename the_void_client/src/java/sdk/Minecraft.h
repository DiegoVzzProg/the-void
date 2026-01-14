#pragma once
#include <jni.h>
#include "../env.h"
#include "../mapper/Mapper.h"
#include "Entity.h"

namespace TheVoid {
	namespace SDK {
		class Minecraft {
		private:
			jobject instance; // objetivo 'Minecraft' real en la memoria.
		public:
			jobject GetInstance() const { return instance; }

			Minecraft(jobject obj) : instance(obj) {}

			static Minecraft GetMinecraft() {
				// 1. Buscamos la clase (ej. "ave" o "net.minecraft.client.Minecraft")
				jclass mcClass = Java::env->FindClass(Java::Mapper::classes[xorstr("Minecraft")].c_str());
				if (!mcClass) return Minecraft(nullptr);

				// 2. Buscamos el m�todo est�tico que devuelve la instancia (a veces es un campo est�tico)
				// En 1.8.9 Vanilla suele ser el m�todo est�tico "A" o "getMinecraft"
				// NOTA: Para este ejemplo usaremos la t�cnica de "Field Est�tico" si existe, o m�todo.
				// Ajustaremos esto seg�n tu versi�n exacta, pero la l�gica general es esta:
				jmethodID getMcMethod = Java::env->GetStaticMethodID(mcClass, xorstr("A"), xorstr("()Lave;"));
				// �OJO! La firma "()Lave;" tambi�n debe ser din�mica en el futuro. 
				// Por ahora, asumiremos Vanilla 1.8.9 est�ndar para probar.

				if (!getMcMethod) return Minecraft(nullptr);

				jobject mcObj = Java::env->CallStaticObjectMethod(mcClass, getMcMethod);
				return Minecraft(mcObj);
			}

			CEntity GetThePlayer() {
				if (!instance) return CEntity(nullptr);

				// Verificamos si el Mapper encontró una firma válida
				if (Java::Mapper::fieldSigs.find(xorstr("Minecraft_thePlayer")) == Java::Mapper::fieldSigs.end()) {
					return CEntity(nullptr); // Si no hay firma, abortamos seguramente.
				}

				jclass mcClass = Java::env->GetObjectClass(instance);

				// Usamos la firma y el nombre que el Mapper ya validó
				jfieldID fid = Java::env->GetFieldID(mcClass,
					Java::Mapper::fields[xorstr("Minecraft_thePlayer")].c_str(),
					Java::Mapper::fieldSigs[xorstr("Minecraft_thePlayer")].c_str() // <--- USO SEGURO
				);

				if (!fid) {
					Java::env->ExceptionClear(); // Por si acaso
					return CEntity(nullptr);
				}

				jobject playerObj = Java::env->GetObjectField(instance, fid);
				return CEntity(playerObj);
			}

			void ClickMouse() {
				if (!instance) return;
				jclass mcClass = Java::env->GetObjectClass(instance);

				const char* methodName = Java::Mapper::methods[xorstr("clickMouse")].c_str();
				const char* methodSig = xorstr("()V");

				jmethodID mid = Java::env->GetMethodID(mcClass, methodName, methodSig);

				if (!mid) {
					std::cout << "[ERROR] Fallo clickMouse. No existe el metodo: '"
						<< methodName << "' con firma '" << methodSig << "'" << std::endl;
					return;
				}

				Java::env->CallVoidMethod(instance, mid);
			}

			bool InGui() {
				if (!instance) return true;

				jclass mcClass = Java::env->GetObjectClass(instance);

				const char* fieldName = Java::Mapper::fields[xorstr("Minecraft_currentScreen")].c_str();
				const char* fieldSig = xorstr("Laxu;"); // Firma de GuiScreen

				jfieldID fid = Java::env->GetFieldID(mcClass, fieldName, fieldSig);

				if (!fid) {
					// Solo imprimimos el error una vez para no spamear
					static bool logged = false;
					if (!logged) {
						std::cout << "[ERROR] Fallo InGui. No existe el campo: '"
							<< fieldName << "' con firma '" << fieldSig << "'" << std::endl;
						logged = true;
					}
					return false;
				}

				jobject screenObj = Java::env->GetObjectField(instance, fid);
				bool inGui = (screenObj != nullptr);
				if (screenObj) Java::env->DeleteLocalRef(screenObj);
				return inGui;
			}

			bool IsLookingAtBlock() {
				if (!instance) return false;
				jclass mcClass = Java::env->GetObjectClass(instance);

				// Check 1: objectMouseOver
				const char* fieldName = Java::Mapper::fields[xorstr("Minecraft_objectMouseOver")].c_str();
				jfieldID fidMouseOver = Java::env->GetFieldID(mcClass, fieldName, xorstr("Lauu;"));

				if (!fidMouseOver) {
					static bool logged = false;
					if (!logged) {
						std::cout << "[ERROR] Fallo SmartMining. No existe campo objectMouseOver: '" << fieldName << "'" << std::endl;
						logged = true;
					}
					return false;
				}

				// (Mantén el resto de tu lógica original aquí abajo si pasa el check)
				jobject mop = Java::env->GetObjectField(instance, fidMouseOver);
				if (!mop) return false;

				// 2. Obtener typeOfHit (campo 'a' que es un Enum MovingObjectType)
				jclass mopClass = Java::env->GetObjectClass(mop);
				jfieldID fidType = Java::env->GetFieldID(
					mopClass,
					Java::Mapper::fields[xorstr("MOP_typeOfHit")].c_str(),
					xorstr("Lauu$a;") // Firma del Enum interno
				);

				jobject enumObj = Java::env->GetObjectField(mop, fidType);
				if (!enumObj) {
					Java::env->DeleteLocalRef(mop);
					return false;
				}

				// 3. Obtener el valor ordinal del Enum (0 = MISS, 1 = BLOCK, 2 = ENTITY)
				// En 1.8.9: BLOCK suele ser index 1.
				jclass enumClass = Java::env->GetObjectClass(enumObj);
				jmethodID midOrdinal = Java::env->GetMethodID(enumClass, xorstr("ordinal"), xorstr("()I"));
				int ordinal = Java::env->CallIntMethod(enumObj, midOrdinal);

				// Limpieza de memoria (CRÍTICO)
				Java::env->DeleteLocalRef(enumClass);
				Java::env->DeleteLocalRef(enumObj);
				Java::env->DeleteLocalRef(mopClass);
				Java::env->DeleteLocalRef(mop);

				// Si el ordinal es 1, es un BLOQUE
				return (ordinal == 1);
			}

			void SetLeftClickCounter(int val) {
				if (!IsValid()) return;

				jclass mcClass = Java::env->GetObjectClass(instance);
				
				static bool messageLeftCount = false;

				if (Java::Mapper::fields.find(xorstr("Minecraft_leftClickCounter")) 
					== Java::Mapper::fields.end()) {
					if (!messageLeftCount) {
						messageLeftCount = true;
						std::cout << "[DEBUG] Fallo al encontrar leftClickCounter" << std::endl;
					}
					return;
				}

				jfieldID fid = Java::env->GetFieldID(
					mcClass,
					Java::Mapper::fields[xorstr("Minecraft_leftClickCounter")].c_str(),
					Java::Mapper::fieldSigs[xorstr("Minecraft_leftClickCounter")].c_str()
				);

				if (fid) {
					Java::env->SetIntField(instance, fid, val);
				}
				else {
					if (!messageLeftCount) {
						messageLeftCount = true;
						std::cout << "[DEBUG] Fallo al encontrar leftClickCounter: pero variable -> fid es null" << std::endl;
					}
				}
			}

			// Verificar si el juego est� listo
			bool IsValid() {
				return instance != nullptr;
			}
		};
	}
}