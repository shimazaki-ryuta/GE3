#pragma once
#include "Vector3.h"
#include "Matrix.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

//worldTransformリソースクラス

struct WorldTransform
{
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
		Matrix4x4 ScaleInverse;
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

	//初期化
	void Initialize();
	//更新
	void UpdateMatrix();
	//ワールド行列取得
	inline Matrix4x4 GetWorld() { return matWorld_; };
	//ワールド座標取得
	Vector3 GetWorldPosition();
	//GPUリソース転送
	void TransfarMatrix(const Matrix4x4&);
};