#pragma once

struct vec2
{
	union
	{
		float data[2];
		struct
		{
			float x;
			float y;
		};
	};

	static const vec2 zero;
};

struct vec3
{
	union
	{
		float data[3];
		struct
		{
			float x;
			float y;
			float z;
		};
	};

	static const vec3 zero;
};

struct vec4
{
	union
	{
		float data[4];
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};

	float& operator[](int i)
	{
		return data[i];
	}

	const float& operator[](int i) const
	{
		return data[i];
	}

	static const vec4 zero;
};

struct mat4
{
	union
	{
		float data[16];
		struct
		{
			vec4 column[4];
		};
	};

	vec4& operator[](int i)
	{
		return column[i];
	}

	const vec4& operator[](int i) const
	{
		return column[i];
	}
};

vec2& operator+=(vec2& lhs, const vec2& rhs);
vec2& operator-=(vec2& lhs, const vec2& rhs);
vec2& operator*=(vec2& lhs, float rhs);
vec2& operator/=(vec2& lhs, float rhs);

vec2 operator+(const vec2& lhs, const vec2& rhs);
vec2 operator-(const vec2& lhs, const vec2& rhs);
vec2 operator*(const vec2& lhs, float rhs);
vec2 operator*(float lhs, const vec2& rhs);
vec2 operator/(const vec2& lhs, float rhs);

vec2 operator-(const vec2& v);

float length(const vec2& v);
vec2 normalize(const vec2& v);

mat4 orthographicProjectionMatrix(float left, float right, float bottom, float top, float zNear, float zFar);
