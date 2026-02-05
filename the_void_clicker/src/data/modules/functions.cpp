#include "functions.h"

namespace Java {
	bool CFunctions::McIsValid() {
		auto mc = Java::CMinecraft::Get().GetJObject();

		return mc != nullptr;
	}

	jobject CFunctions::GetPlayer() {
		return Java::CMinecraft::Get().GetFieldObject(xorstr("Minecraft_thePlayer"));
	}

	jobject CFunctions::GetMouseOver() {
		return Java::CMinecraft::Get().GetFieldObject(xorstr("Minecraft_objectMouseOver"));
	}

	jobject CFunctions::GetCurrentScreen() {
		return Java::CMinecraft::Get().GetFieldObject(xorstr("Minecraft_currentScreen"));
	}

	jobject CFunctions::GetWorld() {
		return Java::CMinecraft::Get().GetFieldObject(xorstr("MC_theWorld"));
	}

	bool CFunctions::IsIngame() {
		if (!McIsValid()) return false;

		jobject world = GetWorld();

		if (world != nullptr) {
			Java::CEnv::Get().env->DeleteLocalRef(world);
			return true;
		}
		return false;
	}

	bool CFunctions::InGui() {
		jobject currentScreen = GetCurrentScreen();

		if (!currentScreen)
			return false;
		else
			return true;
	}

	bool CFunctions::IsInInventory() {
		jobject currentScreen = GetCurrentScreen();
		if (!currentScreen) return false;

		bool esInventario = Java::CMapper::Get().LearnAndCheck(currentScreen, xorstr("INVENTORY"), {
			  xorstr("GuiInventory"),
			  xorstr("GuiContainer"),
			  xorstr("GuiChest"),
			  xorstr("GuiCrafting"),
			  xorstr("adl"),
			  xorstr("ayl"),
			  xorstr("Inventory"),
			  xorstr("wm"),
			  xorstr("InventoryPlayer"),
			  xorstr("field_71071_by"),
			  xorstr("r"),
			}
		);

		Java::CEnv::Get().env->DeleteLocalRef(currentScreen);

		return esInventario;
	}

	bool CFunctions::IsHoldingSword() {
		if (!McIsValid()) {
			return false;
		}

		jobject thePlayer = GetPlayer();
		if (!thePlayer)
		{
			LOG_ERROR(xorstr("No se encontro el objeto player"));
			return false;
		}

		if (Java::CMapper::Get().fields.find(xorstr("Player_inventory")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(thePlayer);
			return false;
		}

		jclass entityPlayerClass = Utils::FindClass(Java::CMapper::Get().classes[xorstr("EntityPlayer")].c_str());
		if (!entityPlayerClass) entityPlayerClass = Java::CEnv::Get().env->GetObjectClass(thePlayer);

		jfieldID invFid = Java::CEnv::Get().env->GetFieldID(entityPlayerClass,
			Java::CMapper::Get().fields[xorstr("Player_inventory")].c_str(),
			Java::CMapper::Get().fieldSigs[xorstr("Player_inventory")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(entityPlayerClass);

		if (!invFid) { Java::CEnv::Get().env->DeleteLocalRef(thePlayer); return false; }

		jobject inventoryObj = Java::CEnv::Get().env->GetObjectField(thePlayer, invFid);
		Java::CEnv::Get().env->DeleteLocalRef(thePlayer);
		if (!inventoryObj) return false;

		// 3. Get Current Item
		jclass invClass = Java::CEnv::Get().env->GetObjectClass(inventoryObj);
		jmethodID getCurrentItemMid = Java::CEnv::Get().env->GetMethodID(invClass,
			Java::CMapper::Get().methods[xorstr("getCurrentItem")].c_str(),
			Java::CMapper::Get().methodSigs[xorstr("getCurrentItem")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(invClass);
		if (!getCurrentItemMid) { Java::CEnv::Get().env->DeleteLocalRef(inventoryObj); return false; }

		jobject itemStack = Java::CEnv::Get().env->CallObjectMethod(inventoryObj, getCurrentItemMid);
		Java::CEnv::Get().env->DeleteLocalRef(inventoryObj);
		if (!itemStack) return false;

		// 4. Get Item Definition
		jclass itemStackClass = Java::CEnv::Get().env->GetObjectClass(itemStack);
		jmethodID getItemMid = Java::CEnv::Get().env->GetMethodID(itemStackClass,
			Java::CMapper::Get().methods[xorstr("getItem")].c_str(),
			Java::CMapper::Get().methodSigs[xorstr("getItem")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(itemStackClass);
		if (!getItemMid) { Java::CEnv::Get().env->DeleteLocalRef(itemStack); return false; }

		jobject itemDef = Java::CEnv::Get().env->CallObjectMethod(itemStack, getItemMid);
		Java::CEnv::Get().env->DeleteLocalRef(itemStack);
		if (!itemDef) return false;

		// 5. CHECK EXCLUSIVO DE ESPADA
		jclass clsSword = Utils::FindClass(Java::CMapper::Get().classes[xorstr("ItemSword")].c_str());
		bool isSword = false;
		if (clsSword) isSword = Java::CEnv::Get().env->IsInstanceOf(itemDef, clsSword);

		Java::CEnv::Get().env->DeleteLocalRef(itemDef);
		if (clsSword) Java::CEnv::Get().env->DeleteLocalRef(clsSword);

		return isSword;
	}

	bool CFunctions::IsHoldingWeapon() {
		if (!McIsValid()) {
			return false;
		}

		jobject thePlayer = GetPlayer();
		if (!thePlayer) {
			LOG_ERROR(xorstr("No se encontro el objeto player"));
			return false;
		}

		// 2. OBTENER INVENTARIO
		// Usamos el Generic del Mapper que configuramos
		if (Java::CMapper::Get().fields.find(xorstr("Player_inventory")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(thePlayer); return false;
		}

		// Truco: Usamos EntityPlayer (Padre) para buscar el ID, ya que es donde vive el campo
		jclass entityPlayerClass = Utils::FindClass(Java::CMapper::Get().classes[xorstr("EntityPlayer")].c_str());
		if (!entityPlayerClass) entityPlayerClass = Java::CEnv::Get().env->GetObjectClass(thePlayer);

		jfieldID invFid = Java::CEnv::Get().env->GetFieldID(entityPlayerClass,
			Java::CMapper::Get().fields[xorstr("Player_inventory")].c_str(),
			Java::CMapper::Get().fieldSigs[xorstr("Player_inventory")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(entityPlayerClass);

		if (!invFid) {
			Java::CEnv::Get().env->DeleteLocalRef(thePlayer); return false;
		}

		jobject inventoryObj = Java::CEnv::Get().env->GetObjectField(thePlayer, invFid);
		Java::CEnv::Get().env->DeleteLocalRef(thePlayer);
		if (!inventoryObj) return false;

		// 3. GET CURRENT ITEM (ItemStack)
		jclass invClass = Java::CEnv::Get().env->GetObjectClass(inventoryObj);
		jmethodID getCurrentItemMid = Java::CEnv::Get().env->GetMethodID(invClass,
			Java::CMapper::Get().methods[xorstr("getCurrentItem")].c_str(),
			Java::CMapper::Get().methodSigs[xorstr("getCurrentItem")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(invClass);

		if (!getCurrentItemMid) {
			Java::CEnv::Get().env->DeleteLocalRef(inventoryObj); return false;
		}

		jobject itemStack = Java::CEnv::Get().env->CallObjectMethod(inventoryObj, getCurrentItemMid);
		Java::CEnv::Get().env->DeleteLocalRef(inventoryObj);

		if (!itemStack) return false; // Mano vacía

		// 4. GET ITEM DEFINITION (Item)
		jclass itemStackClass = Java::CEnv::Get().env->GetObjectClass(itemStack);
		jmethodID getItemMid = Java::CEnv::Get().env->GetMethodID(itemStackClass,
			Java::CMapper::Get().methods[xorstr("getItem")].c_str(),
			Java::CMapper::Get().methodSigs[xorstr("getItem")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(itemStackClass);

		if (!getItemMid) {
			Java::CEnv::Get().env->DeleteLocalRef(itemStack); return false;
		}

		jobject itemDef = Java::CEnv::Get().env->CallObjectMethod(itemStack, getItemMid);
		Java::CEnv::Get().env->DeleteLocalRef(itemStack);

		if (!itemDef) return false;

		// 5. CHECK INSTANCEOF (Espada o Hacha)
		jclass clsSword = Utils::FindClass(Java::CMapper::Get().classes[xorstr("ItemSword")].c_str());
		jclass clsAxe = Utils::FindClass(Java::CMapper::Get().classes[xorstr("ItemAxe")].c_str());

		bool isSword = false;
		bool isAxe = false;

		if (clsSword) isSword = Java::CEnv::Get().env->IsInstanceOf(itemDef, clsSword);
		if (clsAxe)   isAxe = Java::CEnv::Get().env->IsInstanceOf(itemDef, clsAxe);

		// Limpieza final
		Java::CEnv::Get().env->DeleteLocalRef(itemDef);
		if (clsSword) Java::CEnv::Get().env->DeleteLocalRef(clsSword);
		if (clsAxe)   Java::CEnv::Get().env->DeleteLocalRef(clsAxe);

		return (isSword || isAxe);
	}

	bool CFunctions::TargetHasDamage() {
		jobject mop = GetMouseOver();
		if (!mop) {
			LOG_ERROR(xorstr("No encontro el objecto ObjectMouserOver"));
			return false;
		}

		if (Java::CMapper::Get().fields.find(xorstr("MOP_entityHit")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(mop); return false;
		}

		jclass mopClass = Java::CEnv::Get().env->GetObjectClass(mop);
		jfieldID entityHitID = Java::CEnv::Get().env->GetFieldID(mopClass,
			Java::CMapper::Get().fields[xorstr("MOP_entityHit")].c_str(),
			Java::CMapper::Get().fieldSigs[xorstr("MOP_entityHit")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(mopClass);

		if (!entityHitID) { Java::CEnv::Get().env->DeleteLocalRef(mop); return false; }

		jobject targetEntity = Java::CEnv::Get().env->GetObjectField(mop, entityHitID);
		Java::CEnv::Get().env->DeleteLocalRef(mop);

		if (!targetEntity) return false; // Mirando aire

		if (Java::CMapper::Get().fields.find(xorstr("ELB_hurtTime")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(targetEntity); return false;
		}

		jclass elbClass = Utils::FindClass(Java::CMapper::Get().classes[xorstr("EntityLivingBase")].c_str());
		if (!elbClass) {
			elbClass = Java::CEnv::Get().env->GetObjectClass(targetEntity);
		}

		jfieldID hurtTimeID = Java::CEnv::Get().env->GetFieldID(elbClass,
			Java::CMapper::Get().fields[xorstr("ELB_hurtTime")].c_str(),
			xorstr("I")
		);
		Java::CEnv::Get().env->DeleteLocalRef(elbClass);

		if (!hurtTimeID) { Java::CEnv::Get().env->DeleteLocalRef(targetEntity); return false; }

		jint hurtTime = Java::CEnv::Get().env->GetIntField(targetEntity, hurtTimeID);
		Java::CEnv::Get().env->DeleteLocalRef(targetEntity);

		// Si hurtTime > 0, la entidad está parpadeando en rojo (recibió daño)
		return (hurtTime > 0);
	}

	bool CFunctions::IsLookingAtBlock() {
		jobject mop = GetMouseOver();
		if (!mop) {
			LOG_ERROR(xorstr("No se encontro ObjectMouseOver"));
			return false;
		}

		if (Java::CMapper::Get().fields.find(xorstr("MOP_typeOfHit")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(mop);
			return false;
		}

		// 3. Obtener ID (Directo y Limpio)
		jclass mopClass = Java::CEnv::Get().env->GetObjectClass(mop);
		jfieldID fid = Java::CEnv::Get().env->GetFieldID(
			mopClass,
			Java::CMapper::Get().fields[xorstr("MOP_typeOfHit")].c_str(),
			Java::CMapper::Get().fieldSigs[xorstr("MOP_typeOfHit")].c_str()
		);

		Java::CEnv::Get().env->DeleteLocalRef(mopClass);

		if (!fid) {
			Java::CEnv::Get().env->ExceptionClear();
			Java::CEnv::Get().env->DeleteLocalRef(mop);
			return false;
		}

		// 4. Obtener Enum
		jobject enumObj = Java::CEnv::Get().env->GetObjectField(mop, fid);
		Java::CEnv::Get().env->DeleteLocalRef(mop);

		if (!enumObj) return false;

		// 5. Ordinal
		jclass enumClass = Java::CEnv::Get().env->GetObjectClass(enumObj);
		jmethodID midOrdinal = Java::CEnv::Get().env->GetMethodID(enumClass, xorstr("ordinal"), xorstr("()I"));

		int ordinal = -1;
		if (midOrdinal) {
			ordinal = Java::CEnv::Get().env->CallIntMethod(enumObj, midOrdinal);
		}

		Java::CEnv::Get().env->DeleteLocalRef(enumClass);
		Java::CEnv::Get().env->DeleteLocalRef(enumObj);

		return (ordinal == 1);
	}

	bool CFunctions::IsLookingAtEntity() {
		jobject mop = GetMouseOver();
		if (!mop) {
			LOG_ERROR(xorstr("No se encontro ObjectMouseOver"));
			return false;
		}

		if (Java::CMapper::Get().fields.find(xorstr("MOP_entityHit")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(mop); return false;
		}

		jclass mopClass = Java::CEnv::Get().env->GetObjectClass(mop);
		jfieldID entityHitID = Java::CEnv::Get().env->GetFieldID(mopClass,
			Java::CMapper::Get().fields[xorstr("MOP_entityHit")].c_str(),
			Java::CMapper::Get().fieldSigs[xorstr("MOP_entityHit")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(mopClass);

		if (!entityHitID) {
			Java::CEnv::Get().env->DeleteLocalRef(mop); return false;
		}

		// 3. Obtener el objeto
		jobject entityObj = Java::CEnv::Get().env->GetObjectField(mop, entityHitID);
		Java::CEnv::Get().env->DeleteLocalRef(mop);

		// 4. Veredicto
		if (entityObj) {
			// Si el objeto existe, es que estamos mirando a una entidad.
			Java::CEnv::Get().env->DeleteLocalRef(entityObj);
			return true;
		}

		return false; // Es NULL, estamos mirando aire o bloque
	}

	void CFunctions::SetRightClickState(bool pressed) {
		if (!McIsValid()) return;

		static jobject rightClickKeyObj = nullptr;
		static jfieldID pressedFieldID = nullptr;

		if (!rightClickKeyObj) {
			jobject gameSettings = Java::CMinecraft::Get().GetFieldObject(xorstr("MC_gameSettings"));
			if (!gameSettings) return;

			jclass settingsClass = Java::CEnv::Get().env->GetObjectClass(gameSettings);

			// 2. Obtener keyBindUseItem (Click Derecho)
			jfieldID useItemFid = Java::CEnv::Get().env->GetFieldID(settingsClass,
				Java::CMapper::Get().fields[xorstr("keyBindUseItem")].c_str(),
				Java::CMapper::Get().fieldSigs[xorstr("keyBindUseItem")].c_str());

			jobject localKeyObj = Java::CEnv::Get().env->GetObjectField(gameSettings, useItemFid);

			if (localKeyObj) {
				// Guardamos referencia Global
				rightClickKeyObj = Java::CEnv::Get().env->NewGlobalRef(localKeyObj);

				// 3. Obtener el campo 'pressed' (boolean)
				jclass keyClass = Java::CEnv::Get().env->GetObjectClass(localKeyObj);
				pressedFieldID = Java::CEnv::Get().env->GetFieldID(keyClass,
					Java::CMapper::Get().fields[xorstr("pressed")].c_str(),
					xorstr("Z"));

				Java::CEnv::Get().env->DeleteLocalRef(keyClass);
				Java::CEnv::Get().env->DeleteLocalRef(localKeyObj); // Borramos local porque ya tenemos Global
			}

			Java::CEnv::Get().env->DeleteLocalRef(settingsClass);
			Java::CEnv::Get().env->DeleteLocalRef(gameSettings);
		}

		// Ejecución Rápida
		if (rightClickKeyObj && pressedFieldID) {
			Java::CEnv::Get().env->SetBooleanField(rightClickKeyObj, pressedFieldID, pressed ? JNI_TRUE : JNI_FALSE);
		}
	}

	std::vector<jobject> CFunctions::GetWorldPlayers() {
		std::vector<jobject> players;

		// 1. Validar entorno
		if (!McIsValid()) return players;

		// 2. Obtener el Mundo
		jobject theWorld = GetWorld();
		if (!theWorld) return players;

		// 3. Validar que tengamos el mapeo de la lista
		if (Java::CMapper::Get().fields.find(xorstr("World_entityList")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(theWorld);
			return players;
		}

		// 4. Obtener ID del campo 'loadedEntityList'
		jclass worldClass = Java::CEnv::Get().env->GetObjectClass(theWorld);
		jfieldID listID = Java::CEnv::Get().env->GetFieldID(worldClass,
			Java::CMapper::Get().fields[xorstr("World_entityList")].c_str(),
			Java::CMapper::Get().fieldSigs[xorstr("World_entityList")].c_str()
		);
		Java::CEnv::Get().env->DeleteLocalRef(worldClass); // Limpiamos la clase

		if (!listID) {
			Java::CEnv::Get().env->DeleteLocalRef(theWorld);
			return players;
		}

		// 5. Obtener el objeto Lista (java.util.List)
		jobject listObj = Java::CEnv::Get().env->GetObjectField(theWorld, listID);
		Java::CEnv::Get().env->DeleteLocalRef(theWorld); // Ya no necesitamos el mundo

		if (!listObj) return players;

		// 6. Convertir List -> Array (toArray) para iterar en C++
		// Buscamos la clase java/util/List para llamar a toArray()
		jclass listClass = Java::CEnv::Get().env->GetObjectClass(listObj);
		jmethodID toArrayMethod = Java::CEnv::Get().env->GetMethodID(listClass, xorstr("toArray"), xorstr("()[Ljava/lang/Object;"));
		Java::CEnv::Get().env->DeleteLocalRef(listClass);

		if (toArrayMethod) {
			jobjectArray array = (jobjectArray)Java::CEnv::Get().env->CallObjectMethod(listObj, toArrayMethod);
			if (array) {
				int len = Java::CEnv::Get().env->GetArrayLength(array);
				for (int i = 0; i < len; i++) {
					jobject entity = Java::CEnv::Get().env->GetObjectArrayElement(array, i);
					if (entity) {
						// Aquí podrías filtrar si es 'instanceof EntityPlayer' si mapeas esa clase
						// Por ahora guardamos todo, el AimAssist filtrará distancia/tipo
						players.push_back(entity);
					}
				}
				Java::CEnv::Get().env->DeleteLocalRef(array);
			}
		}

		Java::CEnv::Get().env->DeleteLocalRef(listObj);
		return players;
	}

	Vector3 CFunctions::GetEntityPos(jobject entity) {
		Vector3 pos = { 0, 0, 0 };
		if (!entity) return pos;

		// Validamos que existan los mapeos
		if (Java::CMapper::Get().fields.find(xorstr("Entity_posX")) == Java::CMapper::Get().fields.end()) return pos;

		jclass entClass = Java::CEnv::Get().env->GetObjectClass(entity);

		// Obtenemos IDs
		jfieldID xID = Java::CEnv::Get().env->GetFieldID(entClass, Java::CMapper::Get().fields[xorstr("Entity_posX")].c_str(), "D");
		jfieldID yID = Java::CEnv::Get().env->GetFieldID(entClass, Java::CMapper::Get().fields[xorstr("Entity_posY")].c_str(), "D");
		jfieldID zID = Java::CEnv::Get().env->GetFieldID(entClass, Java::CMapper::Get().fields[xorstr("Entity_posZ")].c_str(), "D");

		Java::CEnv::Get().env->DeleteLocalRef(entClass);

		if (xID && yID && zID) {
			pos.x = Java::CEnv::Get().env->GetDoubleField(entity, xID);
			pos.y = Java::CEnv::Get().env->GetDoubleField(entity, yID);
			pos.z = Java::CEnv::Get().env->GetDoubleField(entity, zID);
		}

		return pos;
	}

	float CFunctions::GetHeight(jobject entity) {
		if (!entity) return 0;

		if (Java::CMapper::Get().fields.find(xorstr("Entity_height")) 
			== Java::CMapper::Get().fields.end()) return 0;

		jclass entClass = Java::CEnv::Get().env->GetObjectClass(entity);

		jfieldID height = Java::CEnv::Get().env->GetFieldID(entClass, Java::CMapper::Get().fields[xorstr("Entity_height")].c_str(), xorstr("F"));
		Java::CEnv::Get().env->DeleteLocalRef(entClass);

		if (!height) {
			return 0;
		}

		return Java::CEnv::Get().env->GetFloatField(entity, height);
	}


	Vector3 CFunctions::GetEyePos(jobject entity) {
		Vector3 pos = GetEntityPos(entity);

		return Vector3{
			pos.x,
			(float)(double)(pos.y + (GetHeight(entity) * 0.85)),
			pos.z
		};
	}

	Vector2 CFunctions::GetMyRotation() {
		Vector2 rot = { 0, 0 };
		jobject me = GetPlayer();

		if (!me) return rot;
		if (Java::CMapper::Get().fields.find(xorstr("Entity_rotationYaw")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(me); return rot;
		}

		jclass meClass = Java::CEnv::Get().env->GetObjectClass(me);

		jfieldID yawID = Java::CEnv::Get().env->GetFieldID(meClass, Java::CMapper::Get().fields[xorstr("Entity_rotationYaw")].c_str(), xorstr("F"));
		jfieldID pitchID = Java::CEnv::Get().env->GetFieldID(meClass, Java::CMapper::Get().fields[xorstr("Entity_rotationPitch")].c_str(), xorstr("F"));

		Java::CEnv::Get().env->DeleteLocalRef(meClass);

		if (yawID && pitchID) {
			rot.yaw = Java::CEnv::Get().env->GetFloatField(me, yawID);
			rot.pitch = Java::CEnv::Get().env->GetFloatField(me, pitchID);
		}

		Java::CEnv::Get().env->DeleteLocalRef(me);
		return rot;
	}

	void CFunctions::SetMyRotation(Vector2 rots) {
		if (!McIsValid()) {
			return;
		}

		jobject player = GetPlayer();
		if (!player) {
			return;
		}

		auto& mapper = Java::CMapper::Get();

		// Verificar que los mapeos existan
		if (mapper.fields.find(xorstr("Entity_rotationYaw")) == mapper.fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(player);
			return;
		}
		if (mapper.fields.find(xorstr("Entity_rotationPitch")) == mapper.fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(player);
			return;
		}

		jclass cls = Java::CEnv::Get().env->GetObjectClass(player);

		// Rotación actual
		jfieldID yawID = Java::CEnv::Get().env->GetFieldID(
			cls,
			mapper.fields[xorstr("Entity_rotationYaw")].c_str(),
			xorstr("F")
		);

		jfieldID pitchID = Java::CEnv::Get().env->GetFieldID(
			cls,
			mapper.fields[xorstr("Entity_rotationPitch")].c_str(),
			xorstr("F")
		);

		// Rotación previa (anti-flicker)
		jfieldID prevYawID = nullptr;
		jfieldID prevPitchID = nullptr;

		if (mapper.fields.find(xorstr("Entity_prevRotationYaw")) != mapper.fields.end()) {
			prevYawID = Java::CEnv::Get().env->GetFieldID(
				cls,
				mapper.fields[xorstr("Entity_prevRotationYaw")].c_str(),
				xorstr("F")
			);
		}

		if (mapper.fields.find(xorstr("Entity_prevRotationPitch")) != mapper.fields.end()) {
			prevPitchID = Java::CEnv::Get().env->GetFieldID(
				cls,
				mapper.fields[xorstr("Entity_prevRotationPitch")].c_str(),
				xorstr("F")
			);
		}

		Java::CEnv::Get().env->ExceptionClear();

		float finalPitch = std::clamp(rots.pitch, -90.0f, 90.0f);

		// Escribir rotación actual
		if (yawID) {
			Java::CEnv::Get().env->SetFloatField(player, yawID, rots.yaw);
		}

		if (pitchID) {
			Java::CEnv::Get().env->SetFloatField(player, pitchID, finalPitch);
		}

		// Escribir rotación previa (anti-flicker)
		if (prevYawID) {
			Java::CEnv::Get().env->SetFloatField(player, prevYawID, rots.yaw);
		}

		if (prevPitchID) {
			Java::CEnv::Get().env->SetFloatField(player, prevPitchID, finalPitch);
		}

		Java::CEnv::Get().env->DeleteLocalRef(cls);
		Java::CEnv::Get().env->DeleteLocalRef(player);
	}

	float CFunctions::GetMouseSensitivity() {
		if (!McIsValid()) return 0.5f;

		// 1. Obtener GameSettings (MC_gameSettings)
		jobject gameSettings = Java::CMinecraft::Get().GetFieldObject(xorstr("MC_gameSettings"));
		if (!gameSettings) {
			LOG_INFO("gameSettings invalid 0.5");
			return 0.5f;
		}

		// 2. Obtener el valor float (GS_mouseSensitivity)
		// Usamos el Mapper para asegurar que tenemos los IDs correctos
		if (Java::CMapper::Get().fields.find(xorstr("GS_mouseSensitivity")) == Java::CMapper::Get().fields.end()) {
			Java::CEnv::Get().env->DeleteLocalRef(gameSettings);
			LOG_INFO("GS_mouseSensitivity invalid 0.5");
			return 0.5f;
		}

		jclass gsClass = Java::CEnv::Get().env->GetObjectClass(gameSettings);
		jfieldID sensID = Java::CEnv::Get().env->GetFieldID(gsClass,
			Java::CMapper::Get().fields[xorstr("GS_mouseSensitivity")].c_str(),
			xorstr("F")
		);
		Java::CEnv::Get().env->DeleteLocalRef(gsClass);

		float sensitivity = 0.5f;
		if (sensID) {
			sensitivity = Java::CEnv::Get().env->GetFloatField(gameSettings, sensID);
		}

		Java::CEnv::Get().env->DeleteLocalRef(gameSettings);
		return sensitivity;
	}

	int CFunctions::GetHurtTime(jobject entity) {
		if (!entity || !McIsValid()) return 0;

		if (Java::CMapper::Get().fields.find(xorstr("ELB_hurtTime")) 
			== Java::CMapper::Get().fields.end()) {
			return 0;
		}

		jclass cls = Java::CEnv::Get().env->GetObjectClass(entity);
		jfieldID fid = Java::CEnv::Get().env->GetFieldID(cls,
			Java::CMapper::Get().fields[xorstr("ELB_hurtTime")].c_str(),
			xorstr("I"));

		int ht = 0;
		if (fid) ht = Java::CEnv::Get().env->GetIntField(entity, fid);

		Java::CEnv::Get().env->DeleteLocalRef(cls);
		return ht;
	}

	// [EN FUNCTIONS.CPP]

	float CFunctions::GetSwingProgress(jobject entity) {
		if (!entity || !McIsValid()) return 0.0f;

		if (Java::CMapper::Get().fields.find(xorstr("ELB_swingProgress")) 
			== Java::CMapper::Get().fields.end()) {
			return 0.0f;
		}

		// 2. Obtener ID del campo
		jclass cls = Java::CEnv::Get().env->GetObjectClass(entity);
		jfieldID fid = Java::CEnv::Get().env->GetFieldID(cls, Java::CMapper::Get().fields[xorstr("ELB_swingProgress")].c_str(), "F");

		float progress = 0.0f;

		// 3. Leer valor
		if (fid) {
			progress = Java::CEnv::Get().env->GetFloatField(entity, fid);
		}

		Java::CEnv::Get().env->DeleteLocalRef(cls);
		return progress;
	}
}