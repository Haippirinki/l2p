#include "Math.h"

#include <cmath>

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
	return sqrt(v.x * v.x + v.y * v.y);
}
