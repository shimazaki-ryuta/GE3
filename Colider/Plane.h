#pragma once
#include <Vector3.h>
#include <Matrix.h>
#include <stdint.h>

struct Plane
{
	Vector3 nomal;
	float distance;
};

Vector3 Perpendicular(const Vector3& vector);

void DrawPlane(const Plane& plane,const Matrix4x4& viewProjectionMatrix,const Matrix4x4& viewportMatrix,uint32_t color);
