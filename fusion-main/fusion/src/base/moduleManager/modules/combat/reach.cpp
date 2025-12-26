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


//https://github.com/Lefraudeur/RiptermsGhost

std::chrono::steady_clock::time_point lastUpdate;
void Reach::Update()
{
    static float prev_reach = -1.0f;
    static bool was_enabled = false;

    if (!Enabled)
    {
        if (was_enabled)
        {
            // Cifrado aqu� tambi�n: "reach_distance" no aparecer� en el binario
            Patcher::remove(xorstr("reach_distance"));
            prev_reach = -1.0f;
            was_enabled = false;
        }
        return;
    }

    if (!CommonData::SanityCheck()) return;
    was_enabled = true;

    if (prev_reach != Reach::ReachDistance)
    {
        // Ciframos la clave
        Patcher::put(xorstr("reach_distance"), std::to_string(Reach::ReachDistance));
        prev_reach = Reach::ReachDistance;
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
        Menu::DoSliderStuff(560117, "distance", &Reach::ReachDistance, 3, 3.5);

	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

}
