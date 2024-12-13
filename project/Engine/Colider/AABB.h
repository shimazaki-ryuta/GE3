#pragma once
#include "Vector3.h"
#include "Matrix.h"
#include <stdint.h>

//AABBの当たり判定用構造体

struct AABB
{
	Vector3 min;
	Vector3 max;
};

//AABBを正規化する(最小最大の関係を揃える)
AABB Normalize(const AABB& aabb);
//void DrawAABB(const AABB&, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color);