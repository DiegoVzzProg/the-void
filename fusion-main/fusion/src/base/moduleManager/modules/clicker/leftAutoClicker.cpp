#include "leftAutoClicker.h"
#include "../../../sdk/sdk.h"
#include "../../../menu/menu.h"
#include "../../../util/logger.h"
#include "../../../menu/menu.h"
#include "../../../../../ext/xorstr/xorstr.h"

#include <chrono>
#include <random>
#include "../../../util/math/math.h"

// Variables estáticas para mantener el estado entre frames
static std::mt19937 rng(std::random_device{}());
static bool isMouseDown = false;
static long long nextClickTime = 0;   // Cuándo toca hacer el próximo Click Down
static long long nextReleaseTime = 0; // Cuándo toca hacer el próximo Click Up

long lastClickTime = 0;
int nextCps = 10;

void LeftAutoClicker::Update()
{
    // 1. Validaciones de Seguridad
    if (!Enabled) return;
    if (Menu::Open) return;
    if (SDK::Minecraft->IsInGuiState()) return;
    if (ignoreBlocks && SDK::Minecraft->GetMouseOver().IsTypeOfBlock()) return;

    // Verificar si el usuario físico mantiene el clic izquierdo
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        isMouseDown = false;
        nextClickTime = 0; // Reseteamos el ritmo para que no clickee de golpe al volver a presionar
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

        // Usamos PostMessage para no bloquear el hilo del juego
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

        if (autoBlockHit)
        {
            // Solo intentamos bloquear si tenemos espada (opcional, pero recomendado)
            // if (SDK::Minecraft->thePlayer->IsHoldingSword()) 
            {
                // Tirar dados de probabilidad (ej: 50%)
                std::uniform_int_distribution<int> chanceDist(0, 100);
                if (chanceDist(rng) <= blockhitchance)
                {
                    // Buscar enemigo válido cerca (Rango 4 bloques, FOV 120)
                    CommonData::PlayerData target = GetBestTarget(120.0f, 4.0f);

                    if (target.obj.GetInstance()) { // <--- CAMBIO AQUÍ: Usar GetInstance() como booleano
                        // Enviamos Clic Derecho (Bloquear)
                        PostMessageA(Menu::HandleWindow, WM_RBUTTONDOWN, MK_RBUTTON, 0);

                        // Soltamos el bloqueo
                        PostMessageA(Menu::HandleWindow, WM_RBUTTONUP, 0, 0);
                    }
                }
            }
        }

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
        nextReleaseTime = now + holdTime;       // ¿Cuándo soltamos? Dentro de un rato
        nextClickTime = now + cycleDuration;    // ¿Cuándo inicia el SIGUIENTE clic? Al final del ciclo completo
    }
}

// --- FUNCIÓN PRIVADA DE DETECCIÓN (Copiada y adaptada para vivir aquí) ---
// --- FUNCIÓN PRIVADA DE DETECCIÓN (CORREGIDA) ---
CommonData::PlayerData LeftAutoClicker::GetBestTarget(float fov, float range)
{
    CommonData::PlayerData bestTarget;
    // Inicializamos explícitamente a null/vacío según tu SDK
    // Si 'obj' es un puntero o wrapper, asegurate de que esto compile. 
    // Si da error, simplemente no asignes nada, bestTarget por defecto estará vacía.
    // bestTarget.obj = nullptr; 

    CEntityPlayerSP* thePlayer = SDK::Minecraft->thePlayer;
    if (!thePlayer) return bestTarget;

    Vector3 headPos = thePlayer->GetEyePos();
    Vector3 myPos = thePlayer->GetPos(); // Necesario para calcular distancia manual
    Vector2 currentAngles = thePlayer->GetAngles();

    // CORRECCIÓN 1: Quitamos 'const' para evitar errores de E1086 con GetInstance()
    std::vector<CommonData::PlayerData>& playerList = CommonData::nativePlayerList;

    float finalScore = FLT_MAX;

    // CORRECCIÓN 2: Quitamos 'const' del iterador
    for (auto& player : playerList)
    {
        // CORRECCIÓN 3: Validación segura de punteros
        // Usamos GetInstance() para verificar si es válido
        if (!player.obj.GetInstance()) continue;

        if (Java::Env->IsSameObject(thePlayer->GetInstance(), player.obj.GetInstance())) continue;
        if (!thePlayer->CanEntityBeSeen(player.obj.GetInstance())) continue;

        // CORRECCIÓN 4: Cálculo manual de distancia (Arregla error E0135)
        // Distancia Euclidiana: sqrt((x2-x1)^2 + (y2-y1)^2 + (z2-z1)^2)
        float dx = myPos.x - player.pos.x;
        float dy = myPos.y - player.pos.y;
        float dz = myPos.z - player.pos.z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

        if (dist > range) continue;

        Vector3 targetPos = player.pos + Vector3(0, player.height / 2.0f, 0);
        Vector2 anglesToTarget = Math::getAngles(headPos, targetPos);

        // Matemáticas de ángulo
        Vector2 diff = Math::vec_wrapAngleTo180(currentAngles.Invert() - anglesToTarget.Invert());
        float angleDiff = std::abs(diff.x) + std::abs(diff.y);

        if (angleDiff > fov) continue;

        // Puntuación: Menor es mejor
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
        // --- NUEVA SECCIÓN BLOCKHIT ---
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::Separator();
        ImGui::TextDisabled("Extras"); // Un pequeño título decorativo
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