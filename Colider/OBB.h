#pragma once
#include "Vector3.h"
#include "Matrix.h"
#include <stdint.h>
struct OBB
{
	Vector3 center;
	Vector3 oridentatios[3];
	Vector3 size;
};

void SetOridentatios(OBB& obb, const Matrix4x4& rotateMatrix);
Matrix4x4 GetRotate(const OBB& obb);
//void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color);
