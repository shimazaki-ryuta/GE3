#pragma once
#include <Vector3.h>
#include <Matrix.h>
#include <stdint.h>

//球の当たり判定用構造体

struct Sphere
{
	Vector3 center;//中心点
	float radius;//半径
};
//球同士の当たり判定
bool IsCollision(const Sphere& s1, const Sphere& s2 );
//void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);