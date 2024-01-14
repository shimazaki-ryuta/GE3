#pragma once
#include "Vector3.h"
#include "Matrix.h"
#include <stdint.h>
struct AABB
{
	Vector3 min;
	Vector3 max;
};
AABB Normalize(const AABB& aabb);
void DrawAABB(const AABB&, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color);