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
#include <list>
class Particle
{
public:
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

	struct ParticleForGPU
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 Color;
	};

	struct Transform 
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	struct ParticleData
	{
		Transform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};
	//クラス化までの仮置き
	struct Emitter {
		Transform transform;
		uint32_t count;
		float frequency;
		float frequencyTime;
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
		ID3D12DescriptorHeap* descriptorHeap,
		const std::wstring& directoryPath = L"Resources/");

	void Updade();

	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	void Draw(const ViewProjection& viewProjection);
	static void PostDraw();

	/// <summary>
	/// パーティクル管理用のオブジェクトを生成
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// /// <param name="numInstance">操作できるパーティクル最大数</param>
	static Particle* Create(uint32_t textureHandle, uint32_t numInstance);
	/// <summary>
	/// パーティクル管理用のオブジェクトを生成
	/// </summary>
	/// <param name="numInstance">操作できるパーティクル最大数</param>
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

	void UseBillboard(bool is) { isBillboard_ = is; };

	//パーティクルを一つ生成する
	void MakeNewParticle(const Vector3& translate);
	void MakeNewParticle(const ParticleData& particleData);

	void Emit(const Emitter& emitter);

	std::vector<ParticleData>* GetParticleDate(){ return &particleData_; };

	static const uint32_t kNumInstanceMax = 1000;
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
	ParticleForGPU* instancingData = nullptr;
	//std::vector< struct Transform> transforms;
	std::vector< ParticleData> particleData_;
	Vector4 color_;
	uint32_t textureHandle_;

	Matrix4x4 uvTransform_;

	Matrix4x4 wvp_;

	//BlendMode blendMode_;

	uint32_t numInstance_;
	uint32_t numInstanceMax_;

	D3D12_RESOURCE_DESC resourceDesc_;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

	bool isBillboard_;
};

