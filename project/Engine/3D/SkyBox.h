#pragma once
//メモリリークチェック
#include "D3DResourceLeakChacker.h"
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
#include "ModelStruct.h"

//キューブマップ使用スカイボックス

class SkyBox
{
public:


	struct Material
	{
		Vector4 color;
		Vector3 expornentiation;
	};

	struct OutLineData {
		Vector4 color;
		Matrix4x4 scale;
	};

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	struct CameraForGpu {
		Vector3 worldPosition;
	};


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

	//静的初期化処理
	static void StaticInitialize(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList);
	
	//初期化
	void Initialize(uint32_t texturehandle_);

	//描画
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection);
	
	//Getter/Setter
	uint32_t GetTextureHandle() { return textureHandle_; };

	void SetColor(const Vector4& color) {
		materialData_->color = color;
	};

	void SetExpornentiation(const Vector3& expornentiation) {
		materialData_->expornentiation = expornentiation;
	};

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	VertexDataSkyBox* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	uint32_t textureHandle_;
	
	
};

