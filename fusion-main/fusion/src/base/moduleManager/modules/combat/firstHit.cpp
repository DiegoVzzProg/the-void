#include "firstHit.h"
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

float GetLatencySimulation(float baseAdvantage) {
    // 1. Tiempo actual
    auto now = std::chrono::steady_clock::now();
    long long ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();

    // 2. Inicialización o cambio de objetivo
    // Si ya llegamos al objetivo (o pasó mucho tiempo), elegimos uno nuevo.
    float diff = std::abs(FirstHit::targetOffset - FirstHit::currentOffset);

    // Cambiamos de "estado de red" cada 300-1200ms aleatoriamente
    if (diff < 0.01f || (ms - FirstHit::lastUpdate) >(500 + (rand() % 700))) {
        FirstHit::lastUpdate = ms;

        // Elegimos un nuevo "pico de lag" aleatorio.
        // El factor va de 0.4 (lag bajo) a 1.0 (lag alto/ventaja máxima).
        std::uniform_real_distribution<float> dist(0.4f, 1.0f);
        FirstHit::targetOffset = dist(rng);

        // Velocidad aleatoria (el lag a veces sube rápido, a veces lento)
        std::uniform_real_distribution<float> vel(0.005f, 0.02f);
        FirstHit::velocity = vel(rng);
    }

    // 3. Interpolación Lineal (Suavizado)
    // Movemos el valor actual hacia el objetivo suavemente
    if (FirstHit::currentOffset < FirstHit::targetOffset) {
        FirstHit::currentOffset += FirstHit::velocity;
        if (FirstHit::currentOffset > FirstHit::targetOffset) FirstHit::currentOffset = FirstHit::targetOffset;
    }
    else {
        FirstHit::currentOffset -= FirstHit::velocity;
        if (FirstHit::currentOffset < FirstHit::targetOffset) FirstHit::currentOffset = FirstHit::targetOffset;
    }

    // 4. Cálculo Final
    // Base (3.0) + (Tu Ventaja * Factor Caótico)
    // Agregamos un micro-jitter final para romper la perfección del float
    std::uniform_real_distribution<float> micro_noise(-0.001f, 0.001f);

    return 3.0f + (baseAdvantage * FirstHit::currentOffset) + micro_noise(rng);
}

std::string float_to_string_(float val) {
    char buf[64];
    sprintf_s(buf, "%.4f", val); // 4 decimales de precisión
    std::string s(buf);
    // Reemplaza comas por puntos (fix para Windows en español)
    for (char& c : s) if (c == ',') c = '.';
    return s;
}

void FirstHit::Update() {
    {
        std::lock_guard<std::mutex> lock(Web::candado);
        if (Web::is_new_command && !Web::module_manager_value.is_null()) {
            std::string targetModule = Web::module_manager_value["module"];

            if (targetModule != "first-hit") {
                return;
            }

            FirstHit::Enabled = Web::module_manager_value["action"] == "enable";

            auto& settings = Web::module_manager_value["settings"];
            FirstHit::Range = settings.value("range", 0.00f);

            Web::is_new_command = false;
            Web::module_manager_value = nullptr;
        }
    }

    static float last_sent_val = -1.0f;
    static bool was_active = false;

    // 1. Si está apagado
    if (!FirstHit::Enabled) {
        if (was_active) {
            Patcher::put(xorstr("reach_distance"), xorstr("3.0"));
            last_sent_val = -1.0f;
            was_active = false;
        }
        return;
    }

    if (!CommonData::SanityCheck()) return;
    was_active = true;

    float target_reach = GetLatencySimulation(FirstHit::Range);

    if (std::abs(target_reach - last_sent_val) > 0.0001f) {
        std::string strVal = float_to_string_(target_reach);
        Patcher::put(xorstr("reach_distance"), strVal);
        last_sent_val = target_reach;
    }
}