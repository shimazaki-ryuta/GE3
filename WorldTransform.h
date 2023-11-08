#pragma once
#include "Vector3.h"
#include "Matrix.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>
struct WorldTransform
{
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	// デバイス
	static ID3D12Device* sDevice;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformResource_;
	TransformationMatrix* transformationMatrixData = nullptr;
	static void SetDevice(
		ID3D12Device* device);

	Vector3 scale_ = {1.0f,1.0f,1.0f};
	Vector3 rotation_ = {0.0f,0.0f,0.0f};
	Vector3 translation_ = {0.0f,0.0f,0.0f};
	Matrix4x4 matWorld_;

	const WorldTransform* parent_ = nullptr;

	void Initialize();
	void UpdateMatrix();
	inline Matrix4x4 GetWorld() { return matWorld_; };
	Vector3 GetWorldPosition();
	void TransfarMatrix(const Matrix4x4&);
	Matrix4x4 GetRotate();
};