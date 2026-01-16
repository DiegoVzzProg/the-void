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

			jobject GetFieldObject(std::string fieldKey) {
				if (!instance || !Java::env) return nullptr;

				if (Java::Mapper::fields.find(fieldKey)
					== Java::Mapper::fields.end())
				{
					return nullptr;
				}

				jclass mcClass = Java::env->GetObjectClass(instance);

				const char* fieldName = Java::Mapper::fields[fieldKey].c_str();
				const char* fieldSig = Java::Mapper::fieldSigs[fieldKey].c_str();

				jfieldID fid = Java::env->GetFieldID(mcClass, fieldName, fieldSig);

				if (!fid) {
					Java::env->ExceptionClear(); // Limpiamos error si falla
					Java::env->DeleteLocalRef(mcClass);
					return nullptr;
				}

				jobject Obj = Java::env->GetObjectField(instance, fid);

				Java::env->DeleteLocalRef(mcClass); // Limpieza
				return Obj;
			}

			jobject GetCurrentScreen() {
				return GetFieldObject(xorstr("Minecraft_currentScreen"));
			}


		public:
			jobject GetInstance() const { return instance; }

			Minecraft(jobject obj) : instance(obj) {}

			static Minecraft GetMinecraft() {
				if (!Java::env) return Minecraft(nullptr);

				// Estructura para probar candidatos
				struct Candidate {
					std::string className;
					std::string methodName; // O nombre del campo
					std::string signature;
					bool isMethod; // true = metodo estatico, false = campo estatico
				};

				// LISTA DE PRIORIDAD (El orden importa)
				// 1. Lunar Client / Forge (Lo que usa el otro ghost client)
				// 2. Vanilla 1.8.9 (Método A)
				// 3. Vanilla 1.8.9 (Campo S)
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
						Java::env->ExceptionClear();
						continue; // Si no existe la clase, probamos el siguiente
					}

					jobject mcObj = nullptr;

					if (attempt.isMethod) {
						// INTENTO POR MÉTODO (getMinecraft / A)
						jmethodID mid = Java::env->GetStaticMethodID(cls, attempt.methodName.c_str(), attempt.signature.c_str());
						if (mid) {
							mcObj = Java::env->CallStaticObjectMethod(cls, mid);
							static bool msg1 = false;
							if (!msg1) {
								msg1 = true;
								std::cout << "[THE VOID] Instancia encontrada via METODO: " << attempt.className << "::" << attempt.methodName << std::endl;
							}
						}
					}
					else {
						// INTENTO POR CAMPO (S / theMinecraft)
						jfieldID fid = Java::env->GetStaticFieldID(cls, attempt.methodName.c_str(), attempt.signature.c_str());
						if (fid) {
							mcObj = Java::env->GetStaticObjectField(cls, fid);
							static bool msg2 = false;
							if (!msg2) {
								msg2 = true;
								std::cout << "[THE VOID] Instancia encontrada via CAMPO: " << attempt.className << "::" << attempt.methodName << std::endl;
							}
						}
					}

					// Limpieza de error por si falló este intento específico
					if (!mcObj) Java::env->ExceptionClear();

					// SI LO ENCONTRAMOS...
					if (mcObj) {
						// *** CRÍTICO: ACTUALIZAR EL MAPPER ***
						// Ahora sabemos cuál es la clase real. Forzamos al Mapper a usarla.
						// Esto arregla que el resto del cliente use "ave" o "net/..." correctamente.
						Java::Mapper::classes[xorstr("Minecraft")] = attempt.className;

						Java::env->DeleteLocalRef(cls);
						return Minecraft(mcObj);
					}

					Java::env->DeleteLocalRef(cls);
				}

				static bool msg3 = false;
				if (!msg3) {
					msg3 = true;
					std::cout << "[FATAL] No se pudo obtener la instancia de Minecraft en ningun intento." << std::endl;
				}
				return Minecraft(nullptr);
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

			// [EN MINECRAFT.H]

			bool IsHoldingWeapon() {
				if (!instance || !Java::env) return false;

				// Variable estática para no spamear consola si algo falla (solo avisa 1 vez por error)
				static int failStep = 0;

				// 1. OBTENER JUGADOR (thePlayer)
				jobject thePlayer = GetFieldObject(xorstr("Minecraft_thePlayer"));
				if (!thePlayer) {
					if (failStep != 1) std::cout << "[WEAPON FAIL] Paso 1: Player es nulo." << std::endl;
					failStep = 1;
					return false;
				}

				// 2. OBTENER INVENTARIO
				// Usamos la clase EntityPlayer (Padre) para buscar el ID, ya que ahí lo guardó el Mapper.

				// Verificamos si el Mapper tiene el dato
				if (Java::Mapper::fields.find(xorstr("Player_inventory")) == Java::Mapper::fields.end()) {
					if (failStep != 2) std::cout << "[WEAPON FAIL] El Mapper no encontro 'Player_inventory'. Revisa Mapper::Init." << std::endl;
					failStep = 2;
					Java::env->DeleteLocalRef(thePlayer);
					return false;
				}

				// Cargamos la clase EntityPlayer para pedir el ID
				jclass entityPlayerClass = Utils::FindClass(Java::Mapper::classes[xorstr("EntityPlayer")].c_str());
				if (!entityPlayerClass) {
					// Fallback: intentamos con la clase del objeto actual
					entityPlayerClass = Java::env->GetObjectClass(thePlayer);
				}

				jfieldID invFid = Java::env->GetFieldID(entityPlayerClass,
					Java::Mapper::fields[xorstr("Player_inventory")].c_str(),
					Java::Mapper::fieldSigs[xorstr("Player_inventory")].c_str()
				);

				Java::env->DeleteLocalRef(entityPlayerClass);

				if (!invFid) {
					if (failStep != 21) std::cout << "[WEAPON FAIL] Paso 2: No se encontro el FieldID del inventario." << std::endl;
					failStep = 21;
					Java::env->ExceptionClear();
					Java::env->DeleteLocalRef(thePlayer);
					return false;
				}

				jobject inventoryObj = Java::env->GetObjectField(thePlayer, invFid);
				Java::env->DeleteLocalRef(thePlayer);

				if (!inventoryObj) {
					// Raro, pero posible si el juego está cargando
					failStep = 0;
					return false;
				}

				// 3. OBTENER ITEMSTACK (getCurrentItem)
				jclass invClass = Java::env->GetObjectClass(inventoryObj);

				// Verificamos Mapper
				if (Java::Mapper::methods.find(xorstr("getCurrentItem")) == Java::Mapper::methods.end()) {
					if (failStep != 3) std::cout << "[WEAPON FAIL] Paso 3: Mapper sin 'getCurrentItem'." << std::endl;
					failStep = 3;
					Java::env->DeleteLocalRef(invClass); Java::env->DeleteLocalRef(inventoryObj);
					return false;
				}

				jmethodID getCurrentItemMid = Java::env->GetMethodID(invClass,
					Java::Mapper::methods[xorstr("getCurrentItem")].c_str(),
					Java::Mapper::methodSigs[xorstr("getCurrentItem")].c_str()
				);

				Java::env->DeleteLocalRef(invClass);

				if (!getCurrentItemMid) {
					if (failStep != 31) std::cout << "[WEAPON FAIL] Paso 3.1: JNI no encuentra metodo getCurrentItem." << std::endl;
					failStep = 31;
					Java::env->ExceptionClear(); Java::env->DeleteLocalRef(inventoryObj);
					return false;
				}

				jobject itemStack = Java::env->CallObjectMethod(inventoryObj, getCurrentItemMid);
				Java::env->DeleteLocalRef(inventoryObj);

				if (!itemStack) {
					// Mano vacía -> No es un error, simplemente retornamos false.
					failStep = 0;
					return false;
				}

				// 4. OBTENER ITEM DEFINITION (getItem)
				jclass itemStackClass = Java::env->GetObjectClass(itemStack);

				jmethodID getItemMid = Java::env->GetMethodID(itemStackClass,
					Java::Mapper::methods[xorstr("getItem")].c_str(),
					Java::Mapper::methodSigs[xorstr("getItem")].c_str()
				);

				Java::env->DeleteLocalRef(itemStackClass);

				if (!getItemMid) {
					if (failStep != 4) std::cout << "[WEAPON FAIL] Paso 4: JNI no encuentra metodo getItem." << std::endl;
					failStep = 4;
					Java::env->ExceptionClear(); Java::env->DeleteLocalRef(itemStack);
					return false;
				}

				jobject itemDef = Java::env->CallObjectMethod(itemStack, getItemMid);
				Java::env->DeleteLocalRef(itemStack);

				if (!itemDef) {
					failStep = 0;
					return false;
				}

				// 5. COMPARACIÓN (InstanceOf)
				// Cargamos las clases de Espada y Hacha
				jclass clsSword = Utils::FindClass(Java::Mapper::classes[xorstr("ItemSword")].c_str());
				jclass clsAxe = Utils::FindClass(Java::Mapper::classes[xorstr("ItemAxe")].c_str());

				bool isSword = false;
				bool isAxe = false;

				if (clsSword) isSword = Java::env->IsInstanceOf(itemDef, clsSword);
				if (clsAxe)   isAxe = Java::env->IsInstanceOf(itemDef, clsAxe);

				// Limpieza final
				Java::env->DeleteLocalRef(itemDef);
				if (clsSword) Java::env->DeleteLocalRef(clsSword);
				if (clsAxe)   Java::env->DeleteLocalRef(clsAxe);

				// Éxito
				failStep = 0;
				return (isSword || isAxe);
			}

			bool IsLookingAtBlock() {
				jobject mop = GetFieldObject(xorstr("Minecraft_objectMouseOver"));
				if (!mop) return false;

				// 2. Seguridad Mapper
				if (Java::Mapper::fields.find(xorstr("MOP_typeOfHit")) == Java::Mapper::fields.end()) {
					Java::env->DeleteLocalRef(mop);
					return false;
				}

				// 3. Obtener ID (Directo y Limpio)
				jclass mopClass = Java::env->GetObjectClass(mop);
				jfieldID fid = Java::env->GetFieldID(
					mopClass,
					Java::Mapper::fields[xorstr("MOP_typeOfHit")].c_str(),
					Java::Mapper::fieldSigs[xorstr("MOP_typeOfHit")].c_str()
				);

				Java::env->DeleteLocalRef(mopClass); // Limpieza inmediata de la clase

				if (!fid) {
					Java::env->ExceptionClear();
					Java::env->DeleteLocalRef(mop);
					return false;
				}

				// 4. Obtener Enum
				jobject enumObj = Java::env->GetObjectField(mop, fid);
				Java::env->DeleteLocalRef(mop);

				if (!enumObj) return false;

				// 5. Ordinal
				jclass enumClass = Java::env->GetObjectClass(enumObj);
				jmethodID midOrdinal = Java::env->GetMethodID(enumClass, xorstr("ordinal"), xorstr("()I"));

				int ordinal = -1;
				if (midOrdinal) {
					ordinal = Java::env->CallIntMethod(enumObj, midOrdinal);
				}

				Java::env->DeleteLocalRef(enumClass);
				Java::env->DeleteLocalRef(enumObj);

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