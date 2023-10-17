#pragma once
//メモリリークチェック
#include "D3DResourceLeakChacker.h"
#include "CommonFiles/DirectXCommon.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <vector>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <memory>
class Particle
{
	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct Material
	{
		Vector4 color;
		Matrix4x4 uvTransform;
	};

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	struct ParticleData
	{
		TransformationMatrix transform;
		Vector3 velocity;
	};

	// 頂点数
	static const int kVertNum = 4;
	// デバイス
	static ID3D12Device* sDevice;
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState;

	//static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(BlendMode::CountofBlendMode)> sPipelineStates;

	static void SetsrvDescriptorHeap(ID3D12DescriptorHeap* descriptorHeap) { srvDescriptorHeap_ = descriptorHeap; };

	static ID3D12DescriptorHeap* srvDescriptorHeap_;

	static void StaticInitialize(
		ID3D12Device* device,
		const std::wstring& directoryPath = L"Resources/");

	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	void Draw();
	static void PostDraw();

	static Particle* Create(uint32_t textureHandle, uint32_t numInstance);
	static Particle* Create(uint32_t numInstance);

	void Initialize(uint32_t numInstance);
	void SetTextureHandle(uint32_t textureHandle) { textureHandle_ = textureHandle; };

	inline void SetUVTransform(const Matrix4x4& uvTransform) { uvTransform_ = uvTransform; };

	//inline void SetBlendMode(const BlendMode& blendmode) { blendMode_ = blendmode; };


	//SRVディスクリプタヒープのストラクチャードバッファ使用領域の先頭
	static size_t kSrvStructuredBufferUseBegin;

	//SRVディスクリプタヒープのストラクチャードバッファ使用領域の終端
	static const size_t kSrvStructuredBufferUseEnd = DirectXCommon::kNumSrvDescriptors;

	//static uint32_t CreateStructuredBuffer();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//Microsoft::WRL::ComPtr<ID3D12Resource> transformResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	VertexData* vertexDataSprite_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	TransformationMatrix* instancingData = nullptr;

	Vector4 color_;
	uint32_t textureHandle_;

	Matrix4x4 uvTransform_;

	Matrix4x4 wvp_;

	//BlendMode blendMode_;

	uint32_t numInstance_;

	D3D12_RESOURCE_DESC resourceDesc_;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
};

