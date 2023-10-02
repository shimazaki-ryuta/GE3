#pragma once
struct Vector4
{
	float x, y, z, w;
	Vector4& operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	};
};