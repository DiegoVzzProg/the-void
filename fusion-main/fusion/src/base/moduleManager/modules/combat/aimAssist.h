#pragma once
#include "../../../util/math/geometry.h"
#include <vector>


struct AimAssist
{
    // --- ESTADO EN TIEMPO DE EJECUCIÓN (Runtime State) ---
    // Variables que cambian frame a frame. No se guardan en la config.
    
    // Almacena datos para dibujar líneas de depuración (ESP) sin recalcular.
    inline static Vector3 debugTargetPosition; 
    
    // Datos internos del algoritmo de puntería actual.
    inline static Vector3 currentAimAngles; 
    
    // Bandera para saber si el AimAssist modificó el Pitch (eje Y) en este frame.
    // Útil para no luchar contra el control de retroceso (Recoil Control) si se implementa a futuro.
    inline static bool isPitchInfluenced = false; 


    // --- CONFIGURACIÓN (Settings) ---
    // Todo lo de abajo debería ser guardable en un archivo .json o .cfg

    // Master Switch
    inline static bool Enabled = false;

    // [VISUALS]
    inline static bool visibilityCheck = true;      // Solo apuntar si es visible (Raycast)

    // NO NECESARIOS
    inline static bool aimAssistFeedback = false;    // Dibuja la línea hacia el objetivo
    inline static bool fovCircle = false;            // Dibuja el círculo del FOV

    // [INPUT & BEHAVIOR]
    inline static bool clickOnly = true;            // Reemplaza a "aimKey" para ser más descriptivo. 
                                                    // True: Solo apunta si disparas. False: Apunta siempre (Rage/Blatant).
    // POSIBLEMENTE NO OPCIONES
    inline static bool strafeAssist = true;         // Reemplaza a "adaptive". Predicción de movimiento lateral.
    inline static float strafeOffset = 3.0f;        // Cuánto compensar cuando el jugador se mueve (A/D).

    // [HUMANIZATION - LO MÁS IMPORTANTE PARA SER LEGIT]
    // Un solo valor de 'smooth' es robótico. Usamos un rango [Min, Max].
    inline static float smoothMin = 14.0f;          
    inline static float smoothMax = 18.0f;          
    
    // FOV (Campo de visión): Qué tan cerca de la mira debe estar el enemigo para activarse.
    inline static float fov = 35.0f;
    
    // Distancia máxima en bloques (Minecraft 1.8.9 pvp suele ser 3.0 - 6.0m)
    inline static float aimDistance = 4.0f;         

    // Aleatoriedad (Jitter/Noise) para evitar patrones de detección de "Aim Snap".
    // Aumenté ligeramente el Yaw porque es donde más se nota el movimiento robótico.
    inline static float noiseYaw = 2.5f;            
    inline static float noisePitch = 0.15f;         

    // [SELECTOR DE OBJETIVO]
    inline static int targetPriority = 2; // Por defecto: Closest to Crosshair (El más legit)
    
    // Usamos 'const char* const' para asegurar que los strings sean inmutables en memoria.
    // Esto previene corrupción de memoria accidental y optimiza el uso de la caché.
    inline static const char* const targetPriorityList[3] = { 
        "Distance",
        "Health",
        "Closest to Crosshair"
    };

    // --- MÉTODOS ---

    // Lógica principal llamada cada tick/frame
    static void Update();

    // Renderizado visual (Círculo FOV, Líneas)
    static void RenderUpdate();

    // Interfaz gráfica (ImGui)
    static void RenderMenu();
};