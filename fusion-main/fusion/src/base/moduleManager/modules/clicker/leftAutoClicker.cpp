#include "leftAutoClicker.h"
#include "../../../sdk/sdk.h"
#include "../../../menu/menu.h"
#include "../../../util/logger.h"
#include "../../../../../ext/xorstr/xorstr.h"

#include <random>
#include "../../../util/math/math.h"
#include "../../../web/web.h"

// Variables estáticas para mantener el estado entre frames
static std::mt19937 rng(std::random_device{}());
static bool isMouseDown = false;
static long long nextClickTime = 0;   // Cuándo toca hacer el próximo Click Down
static long long nextReleaseTime = 0; // Cuándo toca hacer el próximo Click Up

long lastClickTime = 0;
int nextCps = 10;

void LeftAutoClicker::Update()
{

    {
        std::lock_guard<std::mutex> lock(Web::candado);

        // Verificamos si hay un comando nuevo Y si el JSON no está vacío
        if (Web::is_new_command && !Web::module_manager_value.is_null()) {

            if (Web::module_manager_value.contains("module")) {

                std::string targetModule = Web::module_manager_value["module"];
                
                if (targetModule == "auto-clicker") {

                    std::cout << "[Clicker] Actualizando configuración..." << std::endl;

                    if (Web::module_manager_value.contains("enabled")) {
                        Enabled = Web::module_manager_value["enabled"];
                    }

                    if (Web::module_manager_value.contains("value")) {
                        float val = Web::module_manager_value["value"];
                        leftMaxCps = val;
                        leftMinCps = (val > 4) ? val - 3 : 1;
                    }

                    Web::is_new_command = false;
                    Web::module_manager_value = nullptr;
                }
            }
        }
    }

    if (!Enabled) return;
    if (Menu::Open) return;
    if (SDK::Minecraft->IsInGuiState()) return;
    if (ignoreBlocks && SDK::Minecraft->GetMouseOver().IsTypeOfBlock()) return;

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


void LeftAutoClicker::AutoBlockHit()
{
    if (!autoBlockHit) return;
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) return;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - LeftAutoClicker::lastBlockTime).count();

    if (elapsed < nextBlockDelay) return;

    CommonData::PlayerData target = GetBestTarget(120.0f, 3.3f);

    if (!target.obj.GetInstance()) return;

    PostMessageA(Menu::HandleWindow, WM_RBUTTONDOWN, MK_RBUTTON, 0);
    PostMessageA(Menu::HandleWindow, WM_RBUTTONUP, 0, 0);

    lastBlockTime = now;

    std::uniform_int_distribution<int> randomDelay(900, 1100);
    nextBlockDelay = randomDelay(rng);
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

        // Solo mostramos el slider de chance si el BlockHit está activo (QoL)
        if (autoBlockHit) {
            Menu::DoSliderStuff(99111, "Block Chance %", &LeftAutoClicker::blockhitchance, 0, 100);
        }
		ImGui::EndChild();
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::EndGroup();
}