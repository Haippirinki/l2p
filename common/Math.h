#pragma once

struct vec2
{
	float x;
	float y;

	static const vec2 zero;
};

struct vec3
{
	float x;
	float y;
	float z;

	static const vec3 zero;
};

struct vec4
{
	float x;
	float y;
	float z;
	float w;

	static const vec4 zero;
};

vec2& operator+=(vec2& lhs, const vec2& rhs);
vec2& operator-=(vec2& lhs, const vec2& rhs);
vec2& operator*=(vec2& lhs, float rhs);
vec2& operator/=(vec2& lhs, float rhs);

bool operator==(const vec2& lhs, const vec2& rhs);
vec2 operator+(const vec2& lhs, const vec2& rhs);
vec2 operator-(const vec2& lhs, const vec2& rhs);
vec2 operator*(const vec2& lhs, float rhs);
vec2 operator*(float lhs, const vec2& rhs);
vec2 operator/(const vec2& lhs, float rhs);

vec2 operator-(const vec2& v);

float length(const vec2& v);
vec2 normalize(const vec2& v);
