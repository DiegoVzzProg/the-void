#include "aimAssist.h"
#include "../../commonData.h"
#include "../../../sdk/sdk.h"
#include "../../../util/logger.h"
#include "../../../../../ext/imgui/imgui.h"
#include "../../../util/math/worldToScreen.h"
#include "../../../util/math/math.h"
#include "../../../menu/menu.h"
#include "../../../java/java.h"

#include <chrono>
#include <random>
#include <cmath>
#include <mutex>
#include "../../../web/web.h"

static float GetRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

void AimAssist::Update()
{
    {
        std::lock_guard<std::mutex> lock(Web::candado);
        if (Web::is_new_command && !Web::module_manager_value.is_null()) {

            std::string targetModule = Web::module_manager_value["module"];

            if (targetModule != "aim-assists") {
                return;
            }

            AimAssist::Enabled = Web::module_manager_value["action"] == "enable";
            
            if (!AimAssist::Enabled) return;

            auto& settings = Web::module_manager_value["settings"];
            AimAssist::aimDistance = settings.value("range", 4.0f);
            AimAssist::fov = settings.value("fov", 35.0f);

            AimAssist::smoothMax = settings.value("smoothness", 15.0f);
            AimAssist::smoothMin = (AimAssist::smoothMax > 4) ? AimAssist::smoothMax - 3 : 1.0f;

            AimAssist::clickOnly = Web::ActivatedCondition("click_only");
            AimAssist::visibilityCheck = Web::ActivatedCondition("visibility_check");
            AimAssist::strafeAssist = Web::ActivatedCondition("strafe_assist");

            if (AimAssist::strafeAssist) {
                AimAssist::strafeOffset = settings["strafe_assist_value"];
            }

            Web::is_new_command = false;
            Web::module_manager_value = nullptr;
        }
    }

    if (!AimAssist::Enabled) return;
    if (!CommonData::SanityCheck()) return;
    if (SDK::Minecraft->IsInGuiState()) return;
    if (Menu::Open) return;

    if (AimAssist::clickOnly && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        AimAssist::debugTargetPosition = Vector3();
        return;
    }

    CEntityPlayerSP* thePlayer = SDK::Minecraft->thePlayer;
    if (!thePlayer) return;

    Vector3 pos = thePlayer->GetPos();
    Vector3 headPos = thePlayer->GetEyePos();
    Vector2 currentLookAngles = thePlayer->GetAngles();

    auto& playerList = CommonData::nativePlayerList;
    if (playerList.empty()) return;

    CommonData::PlayerData target;
    bool targetFound = false;

    float bestDist = FLT_MAX;
    float bestDiff = FLT_MAX;
    float bestHealth = FLT_MAX;
    float maxRange = aimDistance;

    // CORRECCIÓN 3: Quitamos 'const' para evitar el error E1086 con GetInstance()
    for (auto& player : playerList)
    {
        if (Java::Env->IsSameObject(thePlayer->GetInstance(), player.obj.GetInstance())) continue;

        if (visibilityCheck && !thePlayer->CanEntityBeSeen(player.obj.GetInstance())) continue;

        Vector3 playerPos = player.pos;
        float playerHeight = 1.8f;
        Vector3 playerHeadPos = playerPos + Vector3(0, playerHeight - 0.2f, 0);

        Vector2 anglesToHead = Math::getAngles(headPos, playerHeadPos);
        Vector2 difference = Math::vec_wrapAngleTo180(currentLookAngles.Invert() - anglesToHead.Invert());

        float diffYaw = std::abs(difference.x);
        float dist = headPos.Distance(playerPos);

        if (diffYaw <= fov && dist <= maxRange)
        {
            bool isBetterTarget = false;
            switch (targetPriority)
            {
            case 1:
                if (player.health < bestHealth) {
                    bestHealth = player.health;
                    isBetterTarget = true;
                }
                break;
            case 0:
                if (dist < bestDist) {
                    bestDist = dist;
                    isBetterTarget = true;
                }
                break;
            case 2:
            default:
                if (diffYaw < bestDiff) {
                    bestDiff = diffYaw;
                    isBetterTarget = true;
                }
                break;
            }

            if (isBetterTarget) {
                target = player;
                targetFound = true;
            }
        }
    }

    if (!targetFound || !target.obj.GetInstance()) {
        debugTargetPosition = Vector3(); // Limpiar vector visual
        return;
    }

    // --- FASE DE CÁLCULO ---

    Vector3 ePos = target.pos;

    float eHeight = target.height - 0.1f;
    Vector3 eHeadPos = ePos + Vector3(0, eHeight, 0);

    Vector2 anglesFoot = Math::getAngles(headPos, ePos);
    Vector2 anglesHead = Math::getAngles(headPos, eHeadPos);

    Vector2 diffHead = Math::vec_wrapAngleTo180(currentLookAngles.Invert() - anglesHead.Invert());
    Vector2 diffFoot = Math::vec_wrapAngleTo180(currentLookAngles.Invert() - anglesFoot.Invert());

    float yawOffset = GetRandomFloat(-noiseYaw, noiseYaw);

    // CORRECCIÓN 5: Usamos 'strafeAssist' en vez de 'adaptive'
    if (AimAssist::strafeAssist) {
        if ((GetAsyncKeyState('D') & 0x8000) && !(GetAsyncKeyState('A') & 0x8000)) {
            yawOffset -= AimAssist::strafeOffset;
        }
        if ((GetAsyncKeyState('A') & 0x8000) && !(GetAsyncKeyState('D') & 0x8000)) {
            yawOffset += AimAssist::strafeOffset;
        }
    }

    float currentSmooth = GetRandomFloat(smoothMin, smoothMax);
    if (currentSmooth < 1.0f) currentSmooth = 1.0f;

    float targetYaw = currentLookAngles.x + ((diffHead.x + yawOffset) / currentSmooth);

    Vector3 renderPos = CommonData::renderPos;
    float renderPartialTicks = CommonData::renderPartialTicks;

    float targetPitch = currentLookAngles.y;
    isPitchInfluenced = false;

    if (currentLookAngles.y > anglesFoot.y || currentLookAngles.y < anglesHead.y)
    {
        float fixToFoot = currentLookAngles.y + (diffFoot.y / currentSmooth);
        float fixToHead = currentLookAngles.y + (diffHead.y / currentSmooth);

        float absDiffFoot = std::abs(currentLookAngles.y - fixToFoot);
        float absDiffHead = std::abs(currentLookAngles.y - fixToHead);

        if (absDiffFoot > absDiffHead) {
            targetPitch = fixToHead;
            // Actualizar debugTargetPosition
            debugTargetPosition = renderPos - Vector3(0, 0.21f, 0) - (target.lastPos + Vector3(0, eHeight, 0)) + ((target.lastPos - target.pos) * -1.0f * renderPartialTicks);
        }
        else {
            targetPitch = fixToFoot;
            // Actualizar debugTargetPosition
            debugTargetPosition = renderPos - Vector3(0, 0.23f, 0) - target.lastPos + ((target.lastPos - target.pos) * -1.0f * renderPartialTicks);
        }

        // CORRECCIÓN 7: Usamos 'noisePitch' en vez de 'randomPitch'
        targetPitch += GetRandomFloat(-noisePitch, noisePitch);
        isPitchInfluenced = true;
    }
    else {
        // Actualizar debugTargetPosition
        debugTargetPosition = renderPos - target.lastPos + ((target.lastPos - target.pos) * -1.0f * renderPartialTicks);
        targetPitch = currentLookAngles.y + GetRandomFloat(-noisePitch / 2, noisePitch / 2);
    }

    thePlayer->SetAngles(Vector2(targetYaw, targetPitch));
}

void AimAssist::RenderUpdate()
{
    if (!Enabled || !CommonData::dataUpdated) return;

    if (fovCircle) {
        ImVec2 screenSize = ImGui::GetWindowSize();
        float radAimbotFov = (float)(AimAssist::fov * PI / 180);
        float radViewFov = (float)(CommonData::fov * PI / 180);
        float circleRadius = tanf(radAimbotFov / 2) / tanf(radViewFov / 2) * screenSize.x / 1.7325f;

        ImGui::GetWindowDrawList()->AddCircle(ImVec2(screenSize.x / 2, screenSize.y / 2), circleRadius, ImColor(255, 255, 255, 100), 100, 1.0f);
    }

    if (aimAssistFeedback) {
        // Usar debugTargetPosition
        if (std::isnan(debugTargetPosition.x)) return;

        ImVec2 screenSize = ImGui::GetWindowSize();
        Vector2 w2s;

        if (CWorldToScreen::WorldToScreen(debugTargetPosition, CommonData::modelView, CommonData::projection, screenSize.x, screenSize.y, w2s))
        {
            if (std::isnan(w2s.x)) return;
            ImGui::GetWindowDrawList()->AddLine(ImVec2(screenSize.x / 2, screenSize.y / 2), ImVec2(w2s.x, w2s.y), ImColor(0, 255, 255, 200), 1.5f);
        }
    }
}

void AimAssist::RenderMenu()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    if (ImGui::BeginChild("AimAssistSettings", ImVec2(0, 300), true)) {

        Menu::DoToggleButtonStuff(1001, "Enable Aim Assist", &AimAssist::Enabled);
        ImGui::Separator();

        Menu::DoToggleButtonStuff(1002, "Click Only", &AimAssist::clickOnly);
        Menu::DoToggleButtonStuff(1003, "Visibility Check", &AimAssist::visibilityCheck);
        Menu::DoSliderStuff(1004, "FOV", &AimAssist::fov, 1.0f, 90.0f);
        Menu::DoSliderStuff(1005, "Distance", &AimAssist::aimDistance, 3.0f, 6.0f);

        ImGui::Text("Smoothing (Humanization)");
        Menu::DoSliderStuff(1006, "Min Smooth", &AimAssist::smoothMin, 1.0f, 40.0f);
        Menu::DoSliderStuff(1007, "Max Smooth", &AimAssist::smoothMax, 1.0f, 40.0f);

        if (AimAssist::smoothMin > AimAssist::smoothMax) AimAssist::smoothMin = AimAssist::smoothMax;

        ImGui::Separator();

        Menu::DoToggleButtonStuff(1008, "Strafe Assist", &AimAssist::strafeAssist);
        if (AimAssist::strafeAssist) {
            // CORRECCIÓN 8: Nombre actualizado en el menú
            Menu::DoSliderStuff(1009, "Strafe Offset", &AimAssist::strafeOffset, 0.0f, 5.0f);
        }

        // CORRECCIÓN 9: Nombres actualizados en el menú
        Menu::DoSliderStuff(1010, "Noise Yaw (Jitter)", &AimAssist::noiseYaw, 0.0f, 5.0f);
        Menu::DoSliderStuff(1011, "Noise Pitch (Jitter)", &AimAssist::noisePitch, 0.0f, 1.0f);

        ImGui::Separator();

        ImGui::Text("Target Priority");
        ImGui::Combo("##priority", &AimAssist::targetPriority, AimAssist::targetPriorityList, 3);

        ImGui::Separator();

        Menu::DoToggleButtonStuff(1012, "Draw FOV", &AimAssist::fovCircle);
        Menu::DoToggleButtonStuff(1013, "Target Line", &AimAssist::aimAssistFeedback);

        ImGui::EndChild();
    }
    ImGui::PopStyleVar();
}