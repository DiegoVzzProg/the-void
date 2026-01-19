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

			bool IsValid() {
				return instance != nullptr;
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

			// Verifica SOLO si es una espada (Para AutoBlock)
			bool IsHoldingSword() {
				if (!instance || !Java::env) return false;

				// 1. Obtener Jugador
				jobject thePlayer = GetFieldObject(xorstr("Minecraft_thePlayer"));
				if (!thePlayer) return false;

				// 2. Obtener Inventario
				// Usamos el Generic del Mapper
				if (Java::Mapper::fields.find(xorstr("Player_inventory")) == Java::Mapper::fields.end()) {
					Java::env->DeleteLocalRef(thePlayer); return false;
				}

				jclass entityPlayerClass = Utils::FindClass(Java::Mapper::classes[xorstr("EntityPlayer")].c_str());
				if (!entityPlayerClass) entityPlayerClass = Java::env->GetObjectClass(thePlayer);

				jfieldID invFid = Java::env->GetFieldID(entityPlayerClass,
					Java::Mapper::fields[xorstr("Player_inventory")].c_str(),
					Java::Mapper::fieldSigs[xorstr("Player_inventory")].c_str()
				);
				Java::env->DeleteLocalRef(entityPlayerClass);

				if (!invFid) { Java::env->DeleteLocalRef(thePlayer); return false; }

				jobject inventoryObj = Java::env->GetObjectField(thePlayer, invFid);
				Java::env->DeleteLocalRef(thePlayer);
				if (!inventoryObj) return false;

				// 3. Get Current Item
				jclass invClass = Java::env->GetObjectClass(inventoryObj);
				jmethodID getCurrentItemMid = Java::env->GetMethodID(invClass,
					Java::Mapper::methods[xorstr("getCurrentItem")].c_str(),
					Java::Mapper::methodSigs[xorstr("getCurrentItem")].c_str()
				);
				Java::env->DeleteLocalRef(invClass);
				if (!getCurrentItemMid) { Java::env->DeleteLocalRef(inventoryObj); return false; }

				jobject itemStack = Java::env->CallObjectMethod(inventoryObj, getCurrentItemMid);
				Java::env->DeleteLocalRef(inventoryObj);
				if (!itemStack) return false;

				// 4. Get Item Definition
				jclass itemStackClass = Java::env->GetObjectClass(itemStack);
				jmethodID getItemMid = Java::env->GetMethodID(itemStackClass,
					Java::Mapper::methods[xorstr("getItem")].c_str(),
					Java::Mapper::methodSigs[xorstr("getItem")].c_str()
				);
				Java::env->DeleteLocalRef(itemStackClass);
				if (!getItemMid) { Java::env->DeleteLocalRef(itemStack); return false; }

				jobject itemDef = Java::env->CallObjectMethod(itemStack, getItemMid);
				Java::env->DeleteLocalRef(itemStack);
				if (!itemDef) return false;

				// 5. CHECK EXCLUSIVO DE ESPADA
				jclass clsSword = Utils::FindClass(Java::Mapper::classes[xorstr("ItemSword")].c_str());
				bool isSword = false;
				if (clsSword) isSword = Java::env->IsInstanceOf(itemDef, clsSword);

				Java::env->DeleteLocalRef(itemDef);
				if (clsSword) Java::env->DeleteLocalRef(clsSword);

				return isSword;
			}

			bool IsHoldingWeapon() {
				if (!instance || !Java::env) return false;

				// 1. OBTENER JUGADOR
				jobject thePlayer = GetFieldObject(xorstr("Minecraft_thePlayer"));
				if (!thePlayer) return false;

				// 2. OBTENER INVENTARIO
				// Usamos el Generic del Mapper que configuramos
				if (Java::Mapper::fields.find(xorstr("Player_inventory")) == Java::Mapper::fields.end()) {
					Java::env->DeleteLocalRef(thePlayer); return false;
				}

				// Truco: Usamos EntityPlayer (Padre) para buscar el ID, ya que es donde vive el campo
				jclass entityPlayerClass = Utils::FindClass(Java::Mapper::classes[xorstr("EntityPlayer")].c_str());
				if (!entityPlayerClass) entityPlayerClass = Java::env->GetObjectClass(thePlayer);

				jfieldID invFid = Java::env->GetFieldID(entityPlayerClass,
					Java::Mapper::fields[xorstr("Player_inventory")].c_str(),
					Java::Mapper::fieldSigs[xorstr("Player_inventory")].c_str()
				);
				Java::env->DeleteLocalRef(entityPlayerClass);

				if (!invFid) {
					Java::env->DeleteLocalRef(thePlayer); return false;
				}

				jobject inventoryObj = Java::env->GetObjectField(thePlayer, invFid);
				Java::env->DeleteLocalRef(thePlayer);
				if (!inventoryObj) return false;

				// 3. GET CURRENT ITEM (ItemStack)
				jclass invClass = Java::env->GetObjectClass(inventoryObj);
				jmethodID getCurrentItemMid = Java::env->GetMethodID(invClass,
					Java::Mapper::methods[xorstr("getCurrentItem")].c_str(),
					Java::Mapper::methodSigs[xorstr("getCurrentItem")].c_str()
				);
				Java::env->DeleteLocalRef(invClass);

				if (!getCurrentItemMid) {
					Java::env->DeleteLocalRef(inventoryObj); return false;
				}

				jobject itemStack = Java::env->CallObjectMethod(inventoryObj, getCurrentItemMid);
				Java::env->DeleteLocalRef(inventoryObj);

				if (!itemStack) return false; // Mano vacía

				// 4. GET ITEM DEFINITION (Item)
				jclass itemStackClass = Java::env->GetObjectClass(itemStack);
				jmethodID getItemMid = Java::env->GetMethodID(itemStackClass,
					Java::Mapper::methods[xorstr("getItem")].c_str(),
					Java::Mapper::methodSigs[xorstr("getItem")].c_str()
				);
				Java::env->DeleteLocalRef(itemStackClass);

				if (!getItemMid) {
					Java::env->DeleteLocalRef(itemStack); return false;
				}

				jobject itemDef = Java::env->CallObjectMethod(itemStack, getItemMid);
				Java::env->DeleteLocalRef(itemStack);

				if (!itemDef) return false;

				// 5. CHECK INSTANCEOF (Espada o Hacha)
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

			bool IsLookingAtEntity() {
				if (!instance || !Java::env) return false;

				// 1. Obtener objectMouseOver (MOP)
				jobject mop = GetFieldObject(xorstr("Minecraft_objectMouseOver"));
				if (!mop) return false;

				// 2. Obtener FieldID de entityHit
				// Usamos el Generic del Mapper
				if (Java::Mapper::fields.find(xorstr("MOP_entityHit")) == Java::Mapper::fields.end()) {
					Java::env->DeleteLocalRef(mop); return false;
				}

				jclass mopClass = Java::env->GetObjectClass(mop);
				jfieldID entityHitID = Java::env->GetFieldID(mopClass,
					Java::Mapper::fields[xorstr("MOP_entityHit")].c_str(),
					Java::Mapper::fieldSigs[xorstr("MOP_entityHit")].c_str()
				);
				Java::env->DeleteLocalRef(mopClass);

				if (!entityHitID) {
					Java::env->DeleteLocalRef(mop); return false;
				}

				// 3. Obtener el objeto
				jobject entityObj = Java::env->GetObjectField(mop, entityHitID);
				Java::env->DeleteLocalRef(mop);

				// 4. Veredicto
				if (entityObj) {
					// Si el objeto existe, es que estamos mirando a una entidad.
					Java::env->DeleteLocalRef(entityObj);
					return true;
				}

				return false; // Es NULL, estamos mirando aire o bloque
			}

			bool TargetHasDamage() {
				if (!instance || !Java::env) return false;

				// 1. Obtener objectMouseOver
				jobject mop = GetFieldObject(xorstr("Minecraft_objectMouseOver"));
				if (!mop) return false;

				// 2. Obtener entityHit (La entidad apuntada)
				if (Java::Mapper::fields.find(xorstr("MOP_entityHit")) == Java::Mapper::fields.end()) {
					Java::env->DeleteLocalRef(mop); return false;
				}

				// (Usamos la lógica que ya tienes para sacar el FieldID de entityHit...)
				jclass mopClass = Java::env->GetObjectClass(mop);
				jfieldID entityHitID = Java::env->GetFieldID(mopClass,
					Java::Mapper::fields[xorstr("MOP_entityHit")].c_str(),
					Java::Mapper::fieldSigs[xorstr("MOP_entityHit")].c_str()
				);
				Java::env->DeleteLocalRef(mopClass);

				if (!entityHitID) { Java::env->DeleteLocalRef(mop); return false; }

				jobject targetEntity = Java::env->GetObjectField(mop, entityHitID);
				Java::env->DeleteLocalRef(mop);

				if (!targetEntity) return false; // Mirando aire

				// 3. LEER HURT TIME
				// Primero casteamos a EntityLivingBase para acceder al campo
				// (Nota: Si apuntas a un bote o vagoneta crasheará si no validas, 
				// pero IsLookingAtEntity ya filtra bastante).

				if (Java::Mapper::fields.find(xorstr("ELB_hurtTime")) == Java::Mapper::fields.end()) {
					Java::env->DeleteLocalRef(targetEntity); return false;
				}

				// Usamos la clase EntityLivingBase del Mapper
				jclass elbClass = Utils::FindClass(Java::Mapper::classes[xorstr("EntityLivingBase")].c_str());
				if (!elbClass) {
					// Fallback: Usar la clase del objeto mismo
					elbClass = Java::env->GetObjectClass(targetEntity);
				}

				jfieldID hurtTimeID = Java::env->GetFieldID(elbClass,
					Java::Mapper::fields[xorstr("ELB_hurtTime")].c_str(),
					"I" // Firma de int
				);
				Java::env->DeleteLocalRef(elbClass);

				if (!hurtTimeID) { Java::env->DeleteLocalRef(targetEntity); return false; }

				jint hurtTime = Java::env->GetIntField(targetEntity, hurtTimeID);
				Java::env->DeleteLocalRef(targetEntity);

				// Si hurtTime > 0, la entidad está parpadeando en rojo (recibió daño)
				return (hurtTime > 0);
			}

			float GetEntityYaw(jobject entity) {
				if (!entity) return 0.0f;
				// Usamos GetFieldID directo con el mapa del Mapper para velocidad
				jclass cls = Java::env->GetObjectClass(entity);
				jfieldID fid = Java::env->GetFieldID(cls, Java::Mapper::fields[xorstr("Entity_rotationYaw")].c_str(), "F");
				float val = 0.0f;
				if (fid) val = Java::env->GetFloatField(entity, fid);
				Java::env->DeleteLocalRef(cls);
				return val;
			}

			// Obtener Pitch (Arriba/Abajo)
			float GetEntityPitch(jobject entity) {
				if (!entity) return 0.0f;
				jclass cls = Java::env->GetObjectClass(entity);
				jfieldID fid = Java::env->GetFieldID(cls, Java::Mapper::fields[xorstr("Entity_rotationPitch")].c_str(), "F");
				float val = 0.0f;
				if (fid) val = Java::env->GetFloatField(entity, fid);
				Java::env->DeleteLocalRef(cls);
				return val;
			}

			// ESCRIBIR Yaw (Mover la cámara)
			void SetEntityYaw(jobject entity, float yaw) {
				if (!entity) return;
				jclass cls = Java::env->GetObjectClass(entity);
				jfieldID fid = Java::env->GetFieldID(cls, Java::Mapper::fields[xorstr("Entity_rotationYaw")].c_str(), "F");
				if (fid) Java::env->SetFloatField(entity, fid, yaw);
				Java::env->DeleteLocalRef(cls);
			}

			// ESCRIBIR Pitch
			void SetEntityPitch(jobject entity, float pitch) {
				if (!entity) return;
				jclass cls = Java::env->GetObjectClass(entity);
				jfieldID fid = Java::env->GetFieldID(cls, Java::Mapper::fields[xorstr("Entity_rotationPitch")].c_str(), "F");
				if (fid) Java::env->SetFloatField(entity, fid, pitch);
				Java::env->DeleteLocalRef(cls);
			}

			// Obtener altura de ojos (Para apuntar bien)
			float GetEntityEyeHeight(jobject entity) {
				if (!entity) return 0.0f;
				jclass cls = Java::env->GetObjectClass(entity);
				jmethodID mid = Java::env->GetMethodID(cls, Java::Mapper::methods[xorstr("Entity_getEyeHeight")].c_str(), "()F");
				float val = 1.62f; // Default player height
				if (mid) val = Java::env->CallFloatMethod(entity, mid);
				Java::env->DeleteLocalRef(cls);
				return val;
			}

			// --- SENSIBILIDAD DEL MOUSE (Vital para GCD Fix) ---

			float GetMouseSensitivity() {
				jobject mc = GetInstance();
				if (!mc) return 0.5f;

				// 1. Obtener objeto GameSettings
				jclass mcClass = Java::env->GetObjectClass(mc);
				jfieldID gsField = Java::env->GetFieldID(mcClass, Java::Mapper::fields[xorstr("MC_gameSettings")].c_str(), "Lnet/minecraft/client/settings/GameSettings;"); // O usar signature del mapper
				Java::env->DeleteLocalRef(mcClass);

				if (!gsField) { Java::env->DeleteLocalRef(mc); return 0.5f; }

				jobject gameSettings = Java::env->GetObjectField(mc, gsField);
				Java::env->DeleteLocalRef(mc);

				if (!gameSettings) return 0.5f;

				// 2. Obtener float mouseSensitivity
				jclass gsClass = Java::env->GetObjectClass(gameSettings);
				jfieldID sensField = Java::env->GetFieldID(gsClass, Java::Mapper::fields[xorstr("GS_mouseSensitivity")].c_str(), "F");

				float sensitivity = 0.5f;
				if (sensField) {
					sensitivity = Java::env->GetFloatField(gameSettings, sensField);
				}

				Java::env->DeleteLocalRef(gsClass);
				Java::env->DeleteLocalRef(gameSettings);

				return sensitivity;
			}

			// --- VISIBILIDAD (RayTrace Check) ---
			bool CanEntityBeSeen(jobject me, jobject target) {
				if (!me || !target) return false;
				jclass cls = Java::env->GetObjectClass(me);
				jmethodID mid = Java::env->GetMethodID(cls, Java::Mapper::methods[xorstr("Entity_canEntityBeSeen")].c_str(), "(Lnet/minecraft/entity/Entity;)Z");
				bool result = false;
				if (mid) result = Java::env->CallBooleanMethod(me, mid, target);
				Java::env->DeleteLocalRef(cls);
				return result;
			}
		};
	}
}