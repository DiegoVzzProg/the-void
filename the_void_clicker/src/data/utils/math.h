#pragma once
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Vector3
{
public:

	float x{ NAN };
	float y{ NAN };
	float z{ NAN };

	Vector3() {};
	Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
	Vector3 operator + (const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector3 operator + (const float rhs) const { return Vector3(x + rhs, y + rhs, z + rhs); }
	Vector3 operator - (const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	Vector3 operator - (const float rhs) const { return Vector3(x - rhs, y - rhs, z - rhs); }
	Vector3 operator * (const Vector3& rhs) const { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
	Vector3 operator * (const float& rhs) const { return Vector3(x * rhs, y * rhs, z * rhs); }
	Vector3 operator / (const float& rhs) const { return Vector3(x / rhs, y / rhs, z / rhs); }
	Vector3& operator += (const Vector3& rhs) { return *this = *this + rhs; }
	Vector3& operator -= (const Vector3& rhs) { return *this = *this - rhs; }
	Vector3& operator *= (const float& rhs) { return *this = *this * rhs; }
	Vector3& operator /= (const float& rhs) { return *this = *this / rhs; }
	float Length() const { return sqrtf(x * x + y * y + z * z); }
	Vector3 Normalize() const { return *this * (1 / Length()); }
	Vector3 Invert() const { return Vector3{ -x, -y, -z }; }
	float Distance(const Vector3& rhs) const { return (*this - rhs).Length(); }
};

struct Vector2
{
	Vector2 operator - (const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }
	Vector2 Invert() const { return Vector2{ -x, -y }; }
	float x{ NAN };
	float y{ NAN };

	float yaw{ NAN };
	float pitch{ NAN };
};

class Math {
private:
	Math() = default;
public:
	static Math& Get() {
		static Math instance;
		return instance;
	}

	Math(const Math&) = delete;
	void operator=(const Math&) = delete;

	float wrapAngleTo180(float angle);
	Vector2 vec_wrapAngleTo180(Vector2 angle);
	float coterminal(float angle);
	float magnitude(Vector3 vector);
	Vector3 crossProduct(Vector3 p1, Vector3 p2);
	float shortestDistance(Vector3 p, Vector3 a, Vector3 b);
	Vector2 getAngles(Vector3 pos1, Vector3 pos2);
	float radiantsToDeg(float x);
	float degToRadiants(float x);
};