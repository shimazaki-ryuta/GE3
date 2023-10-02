#pragma once
#include "Vector3.h"
#include "Matrix.h"
struct WorldTransform
{

	Vector3 scale_ = {1.0f,1.0f,1.0f};
	Vector3 rotation_ = {0.0f,0.0f,0.0f};
	Vector3 translation_ = {0.0f,0.0f,0.0f};
	Matrix4x4 matWorld_;

	const WorldTransform* parent_ = nullptr;

	void Initialize();
	void UpdateMatrix();
	inline Matrix4x4 GetWorld() { return matWorld_; };
};