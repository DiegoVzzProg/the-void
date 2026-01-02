#include "reach.h"
#include "../../../sdk/sdk.h"
#include "../../commonData.h"
#include "../../../util/logger.h"
#include "../../../java/java.h"
#include "../../../util/math/math.h"
#include "../../../../../ext/imgui/imgui.h"
#include "../../../menu/menu.h"
#include "../../../patcher/patcher.h"
#include "../../../../../ext/xorstr/xorstr.h"

#include <chrono>
#include <cmath>
#include <random>
#include <mutex>
#include "../../../web/web.h"
static std::mt19937 rng(std::random_device{}());

// Función auxiliar robusta para convertir float a string "3.5" (con punto)
std::string float_to_string(float val) {
    char buf[64];
    sprintf_s(buf, "%.4f", val); // 4 decimales de precisión
    std::string s(buf);
    // Reemplaza comas por puntos (fix para Windows en español)
    for (char& c : s) if (c == ',') c = '.';
    return s;
}

static float GetGaussianReach(float min, float max) {
    float mean = (min + max) / 2.0f;
    float sigma = (max - min) / 3.5f;

    std::normal_distribution<float> dist(mean, sigma);
    float val = dist(rng);
    return std::clamp(val, min, max);
}

static bool HitChance(float percentage) {
    if (percentage >= 100.0f) return true;
    if (percentage <= 0.0f) return false;
    std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    return dist(rng) <= percentage;percentage;
}

static float GetMisplaceJitter(float baseMisplace) {
    std::uniform_real_distribution<float> jitter(-0.0002f, 0.0002f);
    // Base (3.0) + Tu ventaja (Delta) + Ruido
    return 3.0f + baseMisplace + jitter(rng);
}

void Reach::Update()
{
    {
        std::lock_guard<std::mutex> lock(Web::candado);
        if (Web::is_new_command && !Web::module_manager_value.is_null()) {
            std::string targetModule = Web::module_manager_value["module"];

            if (targetModule != "reach") {
                return;
            }

            Reach::Enabled = Web::module_manager_value["action"] == "enable";
            
            auto& settings = Web::module_manager_value["settings"];
            Reach::MaxReach = settings.value("range", 3.0f);
            Reach::MinReach = (std::max)(3.01f, Reach::MaxReach - 0.15f);

            Reach::hitChance = settings.value("hitchance", 100.0f);
            
            Reach::Randomization = Web::ActivatedCondition("randomization");
        }
    }

    static float last_sent_val = -1.0f;
    static bool was_active = false;

    // 1. Si está apagado
    if (!Reach::Enabled) {
        if (was_active) {
            Patcher::put(xorstr("reach_distance"), xorstr("3.0"));
            last_sent_val = -1.0f;
            was_active = false;
        }
        return;
    }

    if (!CommonData::SanityCheck()) return;
    was_active = true;

    float target_reach = 3.0f;
    // --- 3. LÓGICA DE DECISIÓN ---
        // Paso A: ¿Tenemos suerte este tick? (Chance)
    if (HitChance(Reach::hitChance))
    {
        // Paso B: ¿Usamos humanización? (Randomize)
        if (Reach::Randomization) {
            target_reach = GetGaussianReach(Reach::MinReach, Reach::MaxReach);
        }
        else {
            target_reach = Reach::MaxReach; // Modo Rage estático
        }
    }
    else
    {
        // Mala suerte: Este golpe será 100% Vanilla (3.0)
        // Esto es vital para engañar a los AntiCheats que buscan patrones constantes.
        target_reach = 3.0f;
    }

    // --- 4. ENVÍO DE DATOS ---
    // Solo enviamos si el valor cambió significativamente (ahorra CPU y JNI)
    if (std::abs(target_reach - last_sent_val) > 0.0001f) {
        std::string strVal = float_to_string(target_reach);
        Patcher::put(xorstr("reach_distance"), strVal);
        last_sent_val = target_reach;

        // Debug (opcional, comenta esto para producción)
        // std::cout << "[Reach] Set: " << strVal << std::endl;
    }
}


void Reach::RenderMenu()
{

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 0.5));
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10);

	if (ImGui::BeginChild("reach", ImVec2(425, 75)))
	{

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
        Menu::DoToggleButtonStuff(230044, "reach", &Reach::Enabled);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
        Menu::DoSliderStuff(560117, "distance", &Reach::MaxReach, 3, 3.5);

	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

}
