#pragma once
#include "Matrix.h"
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
	
	void Initialize();
	void Initialize(int kClientWidth, int kClientHeight);

	void UpdateMatrix();
};