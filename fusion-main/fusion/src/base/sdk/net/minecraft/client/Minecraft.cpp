#include "Minecraft.h"
#include "../../../../java/java.h"
#include "../../../../util/logger.h"

#include "../entity/EntityLivingBase.h"

CMinecraft::CMinecraft()
{
	Java::AssignClass("net.minecraft.client.Minecraft", this->Class);

	this->MethodIDs["getMinecraft"] = Java::Env->GetStaticMethodID(this->GetClass(), "getMinecraft", "()Lnet/minecraft/client/Minecraft;");
	this->Instance = Java::Env->CallStaticObjectMethod(this->GetClass(), this->MethodIDs["getMinecraft"]);

	this->FieldIDs["thePlayer"] = Java::Env->GetFieldID(this->GetClass(), "thePlayer", "Lnet/minecraft/client/entity/EntityPlayerSP;");
	this->thePlayer = new CEntityPlayerSP();

	this->FieldIDs["theWorld"] = Java::Env->GetFieldID(this->GetClass(), "theWorld", "Lnet/minecraft/client/multiplayer/WorldClient;");
	this->theWorld = new CWorldClient();

	this->activeRenderInfo = new CActiveRenderInfo();

	this->FieldIDs["renderManager"] = Java::Env->GetFieldID(this->GetClass(), "renderManager", "Lnet/minecraft/client/renderer/entity/RenderManager;");
	this->renderManager = new CRenderManager();

	this->FieldIDs["timer"] = Java::Env->GetFieldID(this->GetClass(), "timer", "Lnet/minecraft/util/Timer;");
	this->timer = new CTimer();

	this->FieldIDs["gameSettings"] = Java::Env->GetFieldID(this->GetClass(), "gameSettings", "Lnet/minecraft/client/settings/GameSettings;");
	this->gameSettings = new CGameSettings();

	this->MethodIDs["getRenderViewEntity"] = Java::Env->GetMethodID(this->GetClass(), "getRenderViewEntity", "()Lnet/minecraft/entity/Entity;");
	this->FieldIDs["currentScreen"] = Java::Env->GetFieldID(this->GetClass(), "currentScreen", "Lnet/minecraft/client/gui/GuiScreen;");

	this->FieldIDs["objectMouseOver"] = Java::Env->GetFieldID(this->GetClass(), "objectMouseOver", "Lnet/minecraft/util/MovingObjectPosition;");

	// not used lol
	this->MethodIDs["clickMouse"] = Java::Env->GetMethodID(this->GetClass(), "clickMouse", "()V");

	/*this->JFindClass["guiInventory"] = Java::Env->FindClass("net/minecraft/client/gui/inventory/GuiInventory");*/
	this->GuiInventoryClass = nullptr;

}

jclass CMinecraft::GetClass()
{
	return this->Class;
}

jobject CMinecraft::GetInstance()
{
	return this->Instance;
}

CEntity CMinecraft::GetRenderViewEntity()
{
	return CEntity(Java::Env->CallObjectMethod(this->GetInstance(), this->MethodIDs["getRenderViewEntity"]));
}

bool CMinecraft::IsInGuiState()
{
	if (Java::Env->GetObjectField(this->GetInstance(), this->FieldIDs["currentScreen"]) != NULL) return true;
	return false;
}

void CMinecraft::ClickMouse()
{
	Java::Env->CallVoidMethod(this->GetInstance(), this->MethodIDs["clickMouse"]);
}

bool CMinecraft::IsInventory() {	
	// 1. Obtener la pantalla actual
	jobject currentScreenObj = Java::Env->GetObjectField(this->GetInstance(), this->FieldIDs["currentScreen"]);

	// Si es NULL, no estamos en ninguna GUI (jugando o chat cerrado)
	if (!currentScreenObj) return false;

	// --- RUTA RÁPIDA (Una vez detectada la clase) ---
	if (this->GuiInventoryClass != nullptr) {
		bool isInv = Java::Env->IsInstanceOf(currentScreenObj, this->GuiInventoryClass);
		Java::Env->DeleteLocalRef(currentScreenObj);
		return isInv;
	}

	// --- RUTA DE APRENDIZAJE (Solo ocurre la primera vez) ---
	// Si llegamos aquí, es porque aún no hemos capturado la clase GuiInventory.
	// Vamos a inspeccionar qué pantalla es esta.

	jclass cls = Java::Env->GetObjectClass(currentScreenObj);

	// Usamos Reflexión para obtener el nombre (igual que en tu Debug)
	jclass classClass = Java::Env->FindClass("java/lang/Class");
	jmethodID getNameMid = Java::Env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
	jstring nameStr = (jstring)Java::Env->CallObjectMethod(cls, getNameMid);
	const char* nameChars = Java::Env->GetStringUTFChars(nameStr, nullptr);

	// Convertimos a string de C++ para comparar fácilmente
	std::string className(nameChars);

	// Limpieza de strings JNI
	Java::Env->ReleaseStringUTFChars(nameStr, nameChars);
	Java::Env->DeleteLocalRef(nameStr);
	Java::Env->DeleteLocalRef(classClass);

	// [VALIDACIÓN] Comparamos con el nombre que descubriste en el Debug
	// Agregamos también "GuiContainer" por si en el futuro quieres que funcione en cofres.
	bool found = false;
	if (className == "net.minecraft.client.gui.inventory.GuiInventory" ||
		className == "net.minecraft.client.gui.inventory.GuiContainer")
	{
		std::cout << "[The Void] Inventario CAPTURADO: " << className << std::endl;

		// ¡BINGO! Guardamos esta clase como referencia global.
		// Ahora IsInventory funcionará rápido las próximas veces.
		this->GuiInventoryClass = (jclass)Java::Env->NewGlobalRef(cls);
		found = true;
	}

	Java::Env->DeleteLocalRef(cls);
	Java::Env->DeleteLocalRef(currentScreenObj);

	return found;
}

CMovingObjectPosition CMinecraft::GetMouseOver()
{
	return CMovingObjectPosition(Java::Env->GetObjectField(this->Instance, this->FieldIDs["objectMouseOver"]));
}


void CMinecraft::DebugPrintCurrentScreen() {

	// 1. Obtener objeto currentScreen
	jobject currentScreenObj = Java::Env->GetObjectField(this->GetInstance(), this->FieldIDs["currentScreen"]);

	if (!currentScreenObj) {
		std::cout << "[DEBUG] currentScreen es NULL (Jugando o Chat cerrado)" << std::endl;
		return;
	}

	// 2. Obtener la clase del objeto (Reflection: object.getClass())
	jclass cls = Java::Env->GetObjectClass(currentScreenObj);

	// 3. Obtener el método "getName" de java.lang.Class
	jclass classClass = Java::Env->FindClass("java/lang/Class");
	jmethodID getNameMid = Java::Env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");

	// 4. Llamar a getName() -> Devuelve un jstring
	jstring nameStr = (jstring)Java::Env->CallObjectMethod(cls, getNameMid);

	// 5. Convertir a C-String para imprimir
	const char* nameChars = Java::Env->GetStringUTFChars(nameStr, nullptr);

	std::cout << "[DEBUG] CLASE DETECTADA: " << nameChars << std::endl;

	// 6. Limpieza (Vital para no crashear por memoria)
	Java::Env->ReleaseStringUTFChars(nameStr, nameChars);
	Java::Env->DeleteLocalRef(nameStr);
	Java::Env->DeleteLocalRef(classClass);
	Java::Env->DeleteLocalRef(cls);
	Java::Env->DeleteLocalRef(currentScreenObj);
}

std::string GetJNIClassName(jclass cls) {
	jclass classClass = Java::Env->FindClass("java/lang/Class");
	jmethodID getNameMid = Java::Env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
	jstring nameStr = (jstring)Java::Env->CallObjectMethod(cls, getNameMid);
	const char* nameChars = Java::Env->GetStringUTFChars(nameStr, nullptr);
	std::string className(nameChars);
	Java::Env->ReleaseStringUTFChars(nameStr, nameChars);
	Java::Env->DeleteLocalRef(nameStr);
	Java::Env->DeleteLocalRef(classClass);
	return className;
}