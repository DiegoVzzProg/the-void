#include "math.h"
#include <cmath>
#include <algorithm> // Para std::clamp si fuera necesario, o funciones min/max

// Definición de PI de alta precisión si no está definido por <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float Math::wrapAngleTo180(float angle)
{
	angle = std::fmod(angle, 360.0f);

	if (angle >= 180.0f) {
		angle -= 360.0f;
	}
	else if (angle < -180.0f) {
		angle += 360.0f;
	}

	return angle;
}

Vector2 Math::vec_wrapAngleTo180(Vector2 angle)
{
	return Vector2{
		wrapAngleTo180(angle.x),
		wrapAngleTo180(angle.y),
	};
}

float Math::coterminal(float angle) {
	return std::fmod(angle, 360.0f);
}

float Math::magnitude(Vector3 v)
{
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 Math::crossProduct(Vector3 v1, Vector3 v2)
{
	return Vector3{
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

float Math::shortestDistance(Vector3 p, Vector3 a, Vector3 b)
{
	Vector3 ab{ b.x - a.x, b.y - a.y, b.z - a.z };
	Vector3 ap{ p.x - a.x, p.y - a.y, p.z - a.z };

	float magAB = magnitude(ab);

	// SEGURIDAD: Evitar división por cero si A y B son el mismo punto
	if (magAB < 0.0001f) {
		return magnitude(ap);
	}

	Vector3 cp = crossProduct(ap, ab);

	return magnitude(cp) / magAB;
}

Vector2 Math::getAngles(Vector3 pos, Vector3 target)
{
	// Usamos double para precisión intermedia
	double d_x = target.x - pos.x;
	double d_y = target.y - pos.y;
	double d_z = target.z - pos.z;

	// std::hypot es más seguro y rápido que sqrt(x*x + z*z)
	// Calcula la distancia horizontal
	double hypotenuse = std::hypot(d_x, d_z);

	// --- CÁLCULO DE YAW (Horizontal) ---
	// atan2(z, x) devuelve el ángulo en radianes.
	// En matemáticas normales: 0 grados es Este (+X).
	// En Minecraft: 0 grados es Sur (+Z). 
	// Por eso restamos 90 grados.
	float yaw = radiantsToDeg(static_cast<float>(atan2(d_z, d_x))) - 90.0f;

	// --- CÁLCULO DE PITCH (Vertical) ---
	// atan2(y, dist) devuelve el ángulo de elevación.
	// En Minecraft: Mirar arriba es -90, mirar abajo es +90.
	// Por eso invertimos el signo (-).
	float pitch = radiantsToDeg(static_cast<float>(-atan2(d_y, hypotenuse)));

	return Vector2(yaw, pitch);
}

float Math::radiantsToDeg(float x)
{
	return x * 180.0f / static_cast<float>(M_PI);
}

float Math::degToRadiants(float x)
{
	return x * static_cast<float>(M_PI) / 180.0f;
}

