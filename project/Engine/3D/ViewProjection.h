#pragma once
#include "Matrix.h"

//viewProjection行列リソースクラス

struct ViewProjection
{
	Matrix4x4 matView;
	Matrix4x4 matProjection;
	float farZ = 1000.0f;
	float nearZ = 0.1f;
	float width;
	float height;
	float fovAngleY = 0.45f;
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	Vector3 translation_ = { 0.0f,0.0f,0.0f };
	
	//初期化
	void Initialize();
	void Initialize(int kClientWidth, int kClientHeight);

	//更新
	void UpdateMatrix();
};