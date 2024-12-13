#pragma once
#include <Vector3.h>
#include <Matrix.h>
#include <stdint.h>

//平面の当たり判定用構造体

struct Plane
{
	Vector3 nomal;
	float distance;
};

//垂直の時の特殊処理
Vector3 Perpendicular(const Vector3& vector);
//void DrawPlane(const Plane& plane,const Matrix4x4& viewProjectionMatrix,const Matrix4x4& viewportMatrix,uint32_t color);
