#pragma once
#include "Vector3.h"
#include "Matrix.h"
#include <stdint.h>

//OBBの当たり判定用構造体

struct OBB
{
	Vector3 center;
	Vector3 oridentatios[3];
	Vector3 size;
};

//回転行列から軸を設定する
void SetOridentatios(OBB& obb, const Matrix4x4& rotateMatrix);
//軸から回転行列を取得する
Matrix4x4 GetRotate(const OBB& obb);
//void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color);
