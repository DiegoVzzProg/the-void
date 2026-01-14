#pragma once
#include <jni.h>
#include "../env.h"
#include "../mapper/Mapper.h"
#include "Entity.h"

namespace TheVoid {
	namespace SDK {
		class Minecraft {
		private:
			jobject instance;

			jobject GetFieldObject(std::string classKey) {
				if (!instance || !Java::env) return nullptr;

				if (Java::Mapper::fields.find(classKey)
					== Java::Mapper::fields.end())
				{
					return nullptr;
				}

				jclass mcClass = Java::env->GetObjectClass(instance);

				const char* fieldName = Java::Mapper::fields[classKey].c_str();
				const char* fieldSig = Java::Mapper::fieldSigs[classKey].c_str();

				// 3. OBTENER EL FIELD ID
				jfieldID fid = Java::env->GetFieldID(mcClass, fieldName, fieldSig);

				if (!fid) {
					Java::env->ExceptionClear(); // Limpiamos error si falla
					Java::env->DeleteLocalRef(mcClass);
					return nullptr;
				}

				// 4. OBTENER EL OBJETO (Puede ser NULL si estás jugando sin menús)
				jobject screenObj = Java::env->GetObjectField(instance, fid);

				Java::env->DeleteLocalRef(mcClass); // Limpieza
				return screenObj;
			}

			jobject GetCurrentScreen() {
				return GetFieldObject(xorstr("Minecraft_currentScreen"));
			}

		public:
			jobject GetInstance() const { return instance; }

			Minecraft(jobject obj) : instance(obj) {}

			static Minecraft GetMinecraft() {
				// 0. Seguridad básica
				if (!Java::env) return Minecraft(nullptr);

				if (Java::Mapper::classes.find(xorstr("Minecraft")) == Java::Mapper::classes.end()) {
					return Minecraft(nullptr);
				}
				std::string className = Java::Mapper::classes[xorstr("Minecraft")];

				jclass mcClass = Java::env->FindClass(className.c_str());
				if (!mcClass) {
					Java::env->ExceptionClear();
					return Minecraft(nullptr);
				}

				std::string dynamicSig = "()L" + className + ";";

				// 3. BUSCAR EL MÉTODO ESTÁTICO (Entry Point)
				// Aquí probamos las dos variantes más comunes manualmente porque es el inicio de todo.

				// Intento A: Vanilla ("A")
				jmethodID mid = Java::env->GetStaticMethodID(mcClass, xorstr("A"), dynamicSig.c_str());

				// Intento B: Forge ("getMinecraft")
				if (!mid) {
					Java::env->ExceptionClear();
					mid = Java::env->GetStaticMethodID(mcClass, xorstr("getMinecraft"), dynamicSig.c_str());
				}

				if (!mid) {
					Java::env->ExceptionClear();
					Java::env->DeleteLocalRef(mcClass);
					return Minecraft(nullptr);
				}

				jobject mcObj = Java::env->CallStaticObjectMethod(mcClass, mid);
				Java::env->DeleteLocalRef(mcClass);

				return Minecraft(mcObj);
			}

			CEntity GetThePlayer() {
				return CEntity(GetFieldObject(xorstr("Minecraft_thePlayer")));
			}

			void ClickMouse() {
				if (!IsValid()) return;

				// 2. SEGURIDAD: Verificar si el Mapper encontró el nombre del método
								// Si no existe en el mapa, salimos silenciosamente (evita Crash)
				if (Java::Mapper::methods.find(xorstr("clickMouse")) == Java::Mapper::methods.end()) {
					return;
				}

				jclass mcClass = Java::env->GetObjectClass(instance);

				// 3. OBTENER EL NOMBRE DEL MÉTODO DESDE EL MAPPER
				// Aquí es donde la magia ocurre: usará "aw", "av", o "clickMouse" según lo que encontró
				const char* methodName = Java::Mapper::methods[xorstr("clickMouse")].c_str();

				// La firma de clickMouse casi siempre es "()V" (Void, sin argumentos)
				// Si quisieras ser ultra-preciso, podrías guardarla en un mapa 'methodSigs', 
				// pero "()V" es estándar en todas las versiones 1.8.x.
				const char* methodSig = xorstr("()V");

				// 4. OBTENER EL ID DEL MÉTODO
				jmethodID mid = Java::env->GetMethodID(mcClass, methodName, methodSig);

				if (!mid) {
					// Si falla, limpiamos el error de JNI para que el juego no se cierre
					Java::env->ExceptionClear();
					Java::env->DeleteLocalRef(mcClass);
					return;
				}

				// 5. EJECUTAR EL CLICK (GOLPE)
				// CallVoidMethod porque la firma retorna Void (V)
				Java::env->CallVoidMethod(instance, mid);

				// 6. LIMPIEZA
				Java::env->DeleteLocalRef(mcClass);
			}

			bool InGui() {
				jobject currentScreen = GetCurrentScreen();

				if (!currentScreen)
					return false;
				else
					return true;
			}

			bool IsInInventory() {
				jobject currentScreen = GetCurrentScreen();
				if (!currentScreen) return false;

				bool esInventario = Java::Mapper::LearnAndCheck(currentScreen, "INVENTORY",
					{
					  "GuiInventory",  // Forge / Desofuscado
					  "GuiContainer",  // Forge (Cofres)
					  "adl",           // Vanilla 1.8.9 (Inventario)
					  "ayl",           // Vanilla 1.8.9 (Cofre)
					  "Inventory",     // Lunar/Badlion (A veces)
					  "wm",
					  "InventoryPlayer",
					  "field_71071_by",
					}
				);

				Java::env->DeleteLocalRef(currentScreen);

				return esInventario;
			}

			bool IsLookingAtBlock() {
				jobject mop = GetFieldObject(xorstr("Minecraft_objectMouseOver"));
				if (!mop) return false;

				if (Java::Mapper::fields.find(xorstr("MOP_typeOfHit")) == Java::Mapper::fields.end()) {
					Java::env->DeleteLocalRef(mop);
					return false;
				}

				// 2. Obtener typeOfHit (campo 'a' que es un Enum MovingObjectType)
				jclass mopClass = Java::env->GetObjectClass(mop);
				jfieldID fidType = Java::env->GetFieldID(
					mopClass,
					Java::Mapper::fields[xorstr("MOP_typeOfHit")].c_str(),
					Java::Mapper::fieldSigs[xorstr("MOP_typeOfHit")].c_str()
				);

				if (!fidType) {
					Java::env->ExceptionClear();
					Java::env->DeleteLocalRef(mopClass);
					Java::env->DeleteLocalRef(mop);
					return false;
				}

				jobject enumObj = Java::env->GetObjectField(mop, fidType);
				Java::env->DeleteLocalRef(mopClass);
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