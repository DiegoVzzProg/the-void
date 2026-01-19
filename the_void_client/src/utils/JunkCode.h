#pragma once

#pragma once

// ==========================================================
// THE VOID - POLYMORPHIC ENGINE (SOURCE LEVEL)
// ==========================================================

#ifdef _DEBUG
    // EN MODO DEBUG:
    // Desactivamos la basura para que puedas depurar paso a paso sin volverte loco.
#define POLYMORPH() ((void)0)

#else
    // EN MODO RELEASE:
    // Activamos la maquinaria de caos.

    // Usamos volatile para obligar al compilador a NO eliminar este código
    // aunque no sirva para nada útil.
template<int UniqueID>
struct JunkCode {
    static __forceinline void Generate() {
        // Operaciones matemáticas aleatorias basadas en el ID único
        volatile int a = UniqueID;
        volatile int b = a * 1664525 + 1013904223;
        volatile int c = b ^ 0x9e3779b9;
        volatile int d = (c << 6) + (c >> 2);

        // Un poco de flujo de control falso
        if (d % 2 == 0) {
            volatile int e = d + a;
            (void)e;
        }
        else {
            volatile int f = d - a;
            (void)f;
        }
    }
};

// --- MACRO MAESTRA ---
// __COUNTER__: Es una macro de preprocesador que incrementa +1 cada vez que se usa.
// Esto asegura que cada POLYMORPH() genere una clase JunkCode<0>, JunkCode<1>, JunkCode<2>...
// haciendo que el código máquina sea distinto en cada punto.
#define POLYMORPH() JunkCode<__COUNTER__>::Generate()

#endif

/*
    CÓMO USARLO:

    void Update() {
        POLYMORPH(); // <--- Cambia la firma al inicio

        if (condicion) {
            POLYMORPH(); // <--- Inserta basura aquí
            HacerAlgo();
        }
    }
*/