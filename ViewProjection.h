#pragma once
#include "Matrix.h"
struct ViewProjection
{
	Matrix4x4 matView;
	Matrix4x4 matProjection;
	float farZ;
	void Initialize(int kClientWidth, int kClientHeight);


};