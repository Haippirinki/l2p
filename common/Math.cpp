#include "Math.h"

#include <cmath>

const vec2 vec2::zero { 0.f, 0.f };
const vec3 vec3::zero { 0.f, 0.f, 0.f };
const vec4 vec4::zero { 0.f, 0.f, 0.f, 0.f };

vec2& operator+=(vec2& lhs, const vec2& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

vec2& operator-=(vec2& lhs, const vec2& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

vec2& operator*=(vec2& lhs, float rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	return lhs;
}

vec2& operator/=(vec2& lhs, float rhs)
{
	float inv = 1.f / rhs;
	lhs.x *= inv;
	lhs.y *= inv;
	return lhs;
}

vec2 operator+(const vec2& lhs, const vec2& rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}

vec2 operator-(const vec2& lhs, const vec2& rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

vec2 operator*(const vec2& lhs, float rhs)
{
	return { lhs.x * rhs, lhs.y * rhs };
}

vec2 operator*(float lhs, const vec2& rhs)
{
	return { lhs * rhs.x, lhs * rhs.y };
}

vec2 operator/(const vec2& lhs, float rhs)
{
	float inv = 1.f / rhs;
	return { lhs.x * inv, lhs.y * inv };
}

vec2 operator-(const vec2& v)
{
	return { -v.x, -v.y };
}

float length(const vec2& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

vec2 normalize(const vec2& v)
{
	return v / length(v);
}

mat4 orthographicProjectionMatrix(float left, float right, float bottom, float top, float zNear, float zFar)
{
	mat4 r;

	float dx = right - left;
	float dy = top - bottom;
	float dz = zFar - zNear;

	r[0][0] = 2.f / dx;
	r[0][1] = 0.f;
	r[0][2] = 0.f;
	r[0][3] = 0.f;

	r[1][0] = 0.f;
	r[1][1] = 2.f / dy;
	r[1][2] = 0.f;
	r[1][3] = 0.f;

	r[2][0] = 0.f;
	r[2][1] = 0.f;
	r[2][2] = -2.f / dz;
	r[2][3] = 0.f;

	r[3][0] = -(right + left) / dx;
	r[3][1] = -(top + bottom) / dy;
	r[3][2] = -(zFar + zNear) / dz;
	r[3][3] = 1.f;

	return r;
}
