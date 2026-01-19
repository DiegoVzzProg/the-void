#pragma once
#include <cstdint>

// Intentamos incluir el archivo generado. 
// Si no existe (primera compilación), usará la plantilla base sin ofuscación.
#if __has_include("obf_generated.h")
#include "obf_generated.h"
#endif

// Macros para forzar inline (evita que se creen funciones visibles en el ensamblador)
#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE __attribute__((always_inline)) inline
#endif

// Plantilla Base (Fallback por defecto)
// Se usa si el script de Python no ha generado una especialización para este ID
template<typename T, int ID>
class obf {
private:
	T val_;
public:
	FORCE_INLINE obf() : val_(T()) {}
	FORCE_INLINE obf(T v) { val_ = v; }

	// Getters / Setters
	FORCE_INLINE T decrypt() const { return val_; }
	FORCE_INLINE void encrypt(T v) { val_ = v; }

	// Operadores de conversión y asignación
	FORCE_INLINE operator T() const { return decrypt(); }
	FORCE_INLINE obf& operator=(T v) { encrypt(v); return *this; }

	// Operadores matemáticos (para que funcione como una variable normal)
	FORCE_INLINE obf& operator+=(T v) { encrypt(decrypt() + v); return *this; }
	FORCE_INLINE obf& operator-=(T v) { encrypt(decrypt() - v); return *this; }
	FORCE_INLINE obf& operator*=(T v) { encrypt(decrypt() * v); return *this; }
	FORCE_INLINE obf& operator/=(T v) { encrypt(decrypt() / v); return *this; }
	FORCE_INLINE obf& operator++() { encrypt(decrypt() + 1); return *this; }
	FORCE_INLINE obf& operator--() { encrypt(decrypt() - 1); return *this; }
};

// LA MACRO MÁGICA
// Usa __COUNTER__ que incrementa automáticamente cada vez que se llama.
// El script de Python s
#define OBF(Type) obf<Type, __COUNTER__>