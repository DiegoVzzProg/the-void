#include "leftAutoClicker.h"
//#include "../../../sdk/sdk.h"
#include "../../../menu/menu.h"
#include "../../../util/logger.h"
#include "../../../../../ext/xorstr/xorstr.h"

#include <random>
#include "../../../util/math/math.h"
#include "../../../web/web.h"
#include "../../../util/mapper/Mapper.h"

// Variables estáticas para mantener el estado entre frames
static std::mt19937 rng(std::random_device{}());
static bool isMouseDown = false;
static long long nextClickTime = 0;   // Cuándo toca hacer el próximo Click Down
static long long nextReleaseTime = 0; // Cuándo toca hacer el próximo Click Up

void LeftAutoClicker::Update()
{
    //if (GetAsyncKeyState('P') & 1) {
    //    SDK::Minecraft->DebugPrintCurrentScreen();
    //}

    {
        std::lock_guard<std::mutex> lock(Web::candado);

        if (Web::is_new_command && !Web::module_manager_value.is_null()) {

            std::string targetModule = Web::module_manager_value["module"];

            if (targetModule != "auto-clicker") {
                
                return;
            }

            LeftAutoClicker::Enabled = Web::module_manager_value["action"] == "enable";
            
            if (!LeftAutoClicker::Enabled) return;

            auto& settings = Web::module_manager_value["settings"];

            float val = settings.value("cps", 12.0f);
            LeftAutoClicker::leftMaxCps = val;
            LeftAutoClicker::leftMinCps = (val > 4) ? val - 3 : 1;

            LeftAutoClicker::ignoreBlocks = Web::ActivatedCondition("ignore_blocks");

            /*std::cout << LeftAutoClicker::ignoreBlocks << std::endl;*/

            LeftAutoClicker::inventory = Web::ActivatedCondition("inventory");
            Web::is_new_command = false;
            Web::module_manager_value = nullptr;
        }
    }

    if (!LeftAutoClicker::Enabled) return;
    if (Menu::Open) return;
    
    if (!SDK::Minecraft->thePlayer || !SDK::Minecraft->theWorld) return;

    if (SDK::Minecraft->IsInGuiState()) {
        if (SDK::Minecraft->IsInventory())
        {
            if (!LeftAutoClicker::inventory) return;
        }
        else
        {
            return;
        }
    }

    if (LeftAutoClicker::ignoreBlocks && SDK::Minecraft->GetMouseOver().IsTypeOfBlock()) return;

    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        isMouseDown = false;
        nextClickTime = 0;
        return;
    }

    // Obtener tiempo actual en milisegundos
    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    // Si es la primera vez que clickeamos, seteamos el tiempo actual
    if (nextClickTime == 0) nextClickTime = now;

    // 2. Lógica de LEVANTAR (Release / UP)
    // Se ejecuta si el botón virtual está abajo y ya pasó el tiempo de Hold
    if (isMouseDown && now >= nextReleaseTime) {
        POINT pos_cursor;
        GetCursorPos(&pos_cursor);

        PostMessageA(Menu::HandleWindow, WM_LBUTTONUP, 0, MAKELPARAM(pos_cursor.x, pos_cursor.y));
        isMouseDown = false;
    }

    // 3. Lógica de PRESIONAR (Press / DOWN)
    // Se ejecuta si el botón virtual está arriba y ya llegamos al tiempo del siguiente ciclo
    if (!isMouseDown && now >= nextClickTime) {
        POINT pos_cursor;
        GetCursorPos(&pos_cursor);

        PostMessageA(Menu::HandleWindow, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pos_cursor.x, pos_cursor.y));
        isMouseDown = true;

        // --- MATEMÁTICAS CORREGIDAS (DISTRIBUCIÓN REALISTA) ---

        // A. Calcular CPS Objetivo para ESTE clic específico
        // Usamos distribución normal para que oscile (ej: si pides 15, hará 14, 16, 14.5, 15.2...)
        float avgCps = (leftMinCps + leftMaxCps) / 2.0f;
        std::normal_distribution<float> cpsDist(avgCps, 1.5f); // Media, Desviación Estándar
        float thisClickCps = cpsDist(rng);

        // Limitar rangos lógicos
        if (thisClickCps < leftMinCps) thisClickCps = leftMinCps;
        if (thisClickCps > leftMaxCps) thisClickCps = leftMaxCps;

        // B. Calcular cuánto dura el ciclo total en milisegundos
        // Ejemplo: 20 CPS = 50ms total
        long long cycleDuration = (long long)(1000.0f / thisClickCps);

        // C. Calcular Hold Time (Tiempo presionado)
        // Un humano mantiene el clic entre el 40% y 60% del ciclo total, no un tiempo fijo
        std::uniform_real_distribution<float> holdRatioDist(0.4f, 0.6f);
        long long holdTime = (long long)(cycleDuration * holdRatioDist(rng));

        // Validaciones extremas (Hold Time no puede ser menor a 20ms ni mayor al ciclo)
        if (holdTime < 20) holdTime = 20;
        if (holdTime >= cycleDuration) holdTime = cycleDuration - 5;

        // D. Asignar Tiempos Futuros
        nextReleaseTime = now + holdTime;
        nextClickTime = now + cycleDuration;
    }
}

CommonData::PlayerData LeftAutoClicker::GetBestTarget(float fov, float range)
{
    CommonData::PlayerData bestTarget;

    CEntityPlayerSP* thePlayer = SDK::Minecraft->thePlayer;
    if (!thePlayer) return bestTarget;

    Vector3 headPos = thePlayer->GetEyePos();
    Vector3 myPos = thePlayer->GetPos();
    Vector2 currentAngles = thePlayer->GetAngles();

    std::vector<CommonData::PlayerData>& playerList = CommonData::nativePlayerList;

    float finalScore = FLT_MAX;

    for (auto& player : playerList)
    {
        if (!player.obj.GetInstance()) continue;

        if (Java::Env->IsSameObject(thePlayer->GetInstance(), player.obj.GetInstance())) continue;
        if (!thePlayer->CanEntityBeSeen(player.obj.GetInstance())) continue;

        float dx = myPos.x - player.pos.x;
        float dy = myPos.y - player.pos.y;
        float dz = myPos.z - player.pos.z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

        if (dist > range) continue;

        Vector3 targetPos = player.pos + Vector3(0, player.height / 2.0f, 0);
        Vector2 anglesToTarget = Math::getAngles(headPos, targetPos);

        Vector2 diff = Math::vec_wrapAngleTo180(currentAngles.Invert() - anglesToTarget.Invert());
        float angleDiff = std::abs(diff.x) + std::abs(diff.y);

        if (angleDiff > fov) continue;

        float score = angleDiff + (dist * 0.5f);
        if (score < finalScore) {
            finalScore = score;
            bestTarget = player;
        }
    }
    return bestTarget;
}

void LeftAutoClicker::RenderMenu()
{
	ImGui::BeginGroup();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 0.5));
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10);
	if (ImGui::BeginChild("autoclicker", ImVec2(425, 130))) {
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
		Menu::DoToggleButtonStuff(857834, "autoclicker", &LeftAutoClicker::Enabled);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
		Menu::DoSliderStuff(3280, "min cps", &LeftAutoClicker::leftMinCps, 10, LeftAutoClicker::leftMaxCps);
		Menu::DoSliderStuff(675, "max cps", &LeftAutoClicker::leftMaxCps, LeftAutoClicker::leftMinCps, 20);
		Menu::DoToggleButtonStuff(2136, "ignore blocks", &LeftAutoClicker::ignoreBlocks);
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::Separator();
        ImGui::TextDisabled("Extras");
        Menu::DoToggleButtonStuff(99110, "Auto BlockHit", &LeftAutoClicker::autoBlockHit);

		ImGui::EndChild();
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::EndGroup();
}

bool AutoBlockhit::IsHoldingSword() {
    if (!SDK::Minecraft->thePlayer) return false;

    auto inventory = SDK::Minecraft->thePlayer->GetInventory();
    auto currentItem = inventory.GetCurrentItem();

    if (currentItem.GetInstance() == NULL) {
        return false;
    }

    // 2. Inicialización Lazy de la Clase (Solo una vez)
    if (AutoBlockhit::ItemSwordClass == nullptr) {
        // Usamos FindClass estándar para asegurar, o tu wrapper si es seguro
        jclass localClass = Java::Env->FindClass(Mapper::Get("net/minecraft/item/ItemSword", 1).c_str());
        if (localClass) {
            AutoBlockhit::ItemSwordClass = (jclass)Java::Env->NewGlobalRef(localClass);
            Java::Env->DeleteLocalRef(localClass); // Limpieza importante
            std::cout << "[The Void] Clase ItemSword encontrada y cacheada." << std::endl;
        }
        else {
            std::cout << "[ERROR] No se encontro net.minecraft.item.ItemSword" << std::endl;
            Java::Env->ExceptionClear(); // Evita crash si no existe
            return false;
        }
    }

    // 3. Obtener el Item real
    jobject itemObj = currentItem.GetItem();
    if (!itemObj) return false;

    // 4. Chequeo instanceof
    bool isSword = false;
    if (AutoBlockhit::ItemSwordClass) {
        isSword = Java::Env->IsInstanceOf(itemObj, AutoBlockhit::ItemSwordClass);
    }

    if (isSword) {
        // Usamos un contador estático para imprimir solo 1 de cada 60 frames y no laguear
        static int debugCounter = 0;
        if (debugCounter++ % 60 == 0) {
            std::cout << "[The Void] Espada Detectada: TRUE" << std::endl;
        }
    }

    Java::Env->DeleteLocalRef(itemObj);
    return isSword;
}

void AutoBlockhit::InitializeBlockHitCache() {
    
    if (AutoBlockhit::KeyBindingClass != nullptr
        && AutoBlockhit::rightClickKeyObj != nullptr)
    {
        return;
    }

    jobject gameSettings = SDK::Minecraft->gameSettings->GetInstance();
    if (!gameSettings) return;

    jclass settingsClass = Java::Env->GetObjectClass(gameSettings);

    // keyBindUseItem -> "ap" en Vanilla 1.8.9
    jfieldID useItemField = Java::Env->GetFieldID(settingsClass, Mapper::Get("keyBindUseItem", 1).c_str(), Mapper::Get("net/minecraft/client/settings/KeyBinding", 2).c_str());

    jobject localKeyObj = Java::Env->GetObjectField(gameSettings, useItemField);

    if (localKeyObj) {
        // Aprendemos la clase real del objeto (Anti-Lunar)
        jclass detectedClass = Java::Env->GetObjectClass(localKeyObj);

        KeyBindingClass = (jclass)Java::Env->NewGlobalRef(detectedClass);
        rightClickKeyObj = Java::Env->NewGlobalRef(localKeyObj);

        // Field "pressed" -> "i" en Vanilla 1.8.9
        pressedFieldID = Java::Env->GetFieldID(KeyBindingClass, Mapper::Get("pressed", 1).c_str(), "Z");

        std::cout << "[The Void] AutoBlockHit Inicializado Correctamente." << std::endl;
        Java::Env->DeleteLocalRef(detectedClass);
    }

    Java::Env->DeleteLocalRef(gameSettings);
    Java::Env->DeleteLocalRef(settingsClass);
}

void AutoBlockhit::Update() {
    {
        std::lock_guard<std::mutex> lock(Web::candado);
        if (Web::is_new_command && !Web::module_manager_value.is_null()) {

            std::string targetModule = Web::module_manager_value["module"];

            if (targetModule != "auto-clicker") {
                return;
            }

            AutoBlockhit::Enabled = Web::ActivatedCondition("auto_block_hit");

            Web::is_new_command = false;
            Web::module_manager_value = nullptr;
        }
    }

    if (!LeftAutoClicker::Enabled 
        || !AutoBlockhit::Enabled) return;

    if (!SDK::Minecraft->thePlayer 
        || !SDK::Minecraft->theWorld) return;

    if (SDK::Minecraft->IsInGuiState()) return;

    bool leftClickDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

    // Si NO estamos atacando, asegurarnos de soltar el bloqueo
    // Esto es vital: si no lo soltamos, el jugador se quedará bloqueando para siempre.
    if (!leftClickDown) {
        if (AutoBlockhit::isBlockingInternal 
            && AutoBlockhit::rightClickKeyObj) 
        {
            Java::Env->SetBooleanField(AutoBlockhit::rightClickKeyObj, AutoBlockhit::pressedFieldID, JNI_FALSE);
            AutoBlockhit::isBlockingInternal = false;
        }
        return;
    }

    // 2. Inicializar Cache si hace falta
    if (!AutoBlockhit::rightClickKeyObj) {
        AutoBlockhit::InitializeBlockHitCache();
        return;
    }

    // 3. Verificar si tiene Espada (Anti-Ban)
    // Si estás pegando con la mano, NO bloquees.
    if (!AutoBlockhit::IsHoldingSword()) {
        // Si cambiamos a la mano mientras bloqueabamos, soltar bloqueo.
        if (AutoBlockhit::isBlockingInternal) {
            Java::Env->SetBooleanField(AutoBlockhit::rightClickKeyObj, AutoBlockhit::pressedFieldID, JNI_FALSE);
            AutoBlockhit::isBlockingInternal = false;
        }
        return;
    }

    static auto lastBlockTime = std::chrono::steady_clock::now();
    static int nextDelay = 100;

    auto now = std::chrono::steady_clock::now();
    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlockTime).count();

    if (elapsed > nextDelay) {
        std::cout << "[The Void] >> EXECUTE BLOCKHIT (JNI Send)" << std::endl;

        Java::Env->SetBooleanField(AutoBlockhit::rightClickKeyObj, AutoBlockhit::pressedFieldID, JNI_TRUE);
        AutoBlockhit::isBlockingInternal = true;

        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(AutoBlockhit::MinDelay, AutoBlockhit::MaxDelay);
        nextDelay = dist(rng);
        lastBlockTime = now;
    }
}