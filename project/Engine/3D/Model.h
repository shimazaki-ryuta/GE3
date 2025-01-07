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
//#include <stdfloat>
#include "ModelStruct.h"
#include "Material.h"

//3Dモデルクラス

class Model
{
public:
	
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

	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignatureOutLine;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineStateOutLine;

	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignatureSkinning;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineStateSkinning;

	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignatureSkinningOutLine;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineStateSkinningOutLine;

	//静的初期化
	static void StaticInitialize(
		ID3D12Device* device);
	//静的初期化(アウトライン関係)
	static void StaticInitializeOutLine(
		ID3D12Device* device);
	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	//描画前処理(アウトライン)
	static void PreDrawOutLine(ID3D12GraphicsCommandList* cmdList);
	//描画後処理
	static void PostDraw();

	//初期化処理
	void Create(const  std::string& directoryPath, const std::string& filename);

	//初期化処理(meshSync用)
	void CreateTerrain(const  std::string& directoryPath, const std::string& filename);

	//objファイルから生成
	static Model* CreateFromOBJ(const  std::string& directoryPath);

	//頂点,インデックスバッファーにデータ転送
	void TransferBuffer();

	//描画
	void Draw(WorldTransform& worldTransform,const ViewProjection& viewProjection);
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection,uint32_t textureHandle);

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, ID3D12Resource* animationMatrixResource);

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, SkinCluster& skinCluster);

	void DrawOutLine(WorldTransform& worldTransform, const ViewProjection& viewProjection);
	void DrawOutLine(WorldTransform& worldTransform, const ViewProjection& viewProjection, SkinCluster& skinCluster);

	//Getter/Setter
	void SetEnableLighting(int32_t enableLigthing) {materialData_->enableLighting = enableLigthing;};
	void SetShiniess(float shininess) { materialData_->shininess = shininess; };
	void SetEnvironmentCoefficient(float scale) { materialData_->environmentCoefficient = scale; };
	void SetOutLineColor(const Vector4& color) { outlineData_->color = color; };
	void SetOutLineWidth(const Vector3& wid) { outlineData_->scale = MakeScaleMatrix(wid); };
	//シェーディングタイプ 0:通常 1:2値化 
	void SetShadingType(int32_t num) { materialData_->shadingType = num; };
	void SetToonShadowTextureHandle(uint32_t handle) { toonShadowTextureHandle_ = handle; };
	void SetGrowStrength(float strength) { materialData_->growStrength = strength; };

	void SetLocalMatrix(const Matrix4x4& newMatrix) { *localMatrixData_ = newMatrix; };
	
	Node& GetRootNode() { return modelData_.rootNode; };
	ModelData& GetModelData(){ return modelData_; };

	void SetPerspectivTextureHandle(uint32_t handle) { perspectivTextureHandle_ = handle; };

	void SetMaterial(Material* material) { 
		materialResourcePtr_ = material->GetMaterialResource(); 
		outlineResourcePtr_ = material->GetOutlineResource();
	};
	void SetInnerMaterial() {
		materialResourcePtr_ = materialResource_.Get(); 
		outlineResourcePtr_ = outlineResource_.Get();
	};

	VertexData* GetVertexData() { return vertexData_; };

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	ID3D12Resource* materialResourcePtr_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> outlineResource_;
	ID3D12Resource* outlineResourcePtr_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	MaterialParamater* materialData_ = nullptr;
	OutLineData* outlineData_ = nullptr;
	CameraForGpu* cameraData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> localMatrixResource_;
	Matrix4x4* localMatrixData_ = nullptr;
	WorldTransform worldTransform_;
	Vector2 position_;
	Vector2 size_;
	Vector4 color_;
	uint32_t textureHandle_;
	uint32_t toonShadowTextureHandle_ = 1;
	uint32_t disolveMaskTextureHandle_;
	Matrix4x4 uvTransform_;

	Matrix4x4 wvp_;

	D3D12_RESOURCE_DESC resourceDesc_;

	size_t vertexNum = 0;
	std::vector<MeshData*> meshs_;

	ModelData modelData_;
	static void CreateRootSignatureSkinning();
	static void CreateRootSignatureSkinningOutLine();

	//遠景cubeMap
	uint32_t perspectivTextureHandle_;
};

