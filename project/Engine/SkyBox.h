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
class SkyBox
{
public:


	struct Material
	{
		Vector4 color;
		
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

	static void StaticInitialize(
		ID3D12Device* device, int window_width, int window_height,
		ID3D12GraphicsCommandList* cmdList);
	//static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	//static void PostDraw();

	void Initialize(uint32_t texturehandle_);

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection);
	

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

