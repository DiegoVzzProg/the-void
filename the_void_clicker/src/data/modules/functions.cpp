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
}