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
class Model
{
public:
	//static D3DResourceLeakChacker* leakchecker;
	//static std::shared_ptr<D3DResourceLeakChacker> leakchecker;
	//D3DResourceLeakChacker* leakchecker = D3DResourceLeakChacker::GetInstance();
	//leakchecker.reset(D3DResourceLeakChacker::GetInstance());
	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	struct CameraForGpu {
		Vector3 worldPosition;
	};

	struct MaterialData
	{
		std::string textureFilePath;
		uint32_t textureHandle;
	};

	struct MeshData
	{
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	struct ModelData {
		//std::vector<VertexData> vertices;
		//MaterialData material;
		//std::vector<MeshData> meshs;
		MeshData meshs;
		size_t vertexNum;
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
		const std::wstring& directoryPath = L"Resources/");

	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	
	static void PostDraw();

	void Create(const  std::string& directoryPath, const std::string& filename);

	static Model* CreateFromOBJ(const  std::string& directoryPath);

	void Draw(WorldTransform& worldTransform,const ViewProjection& viewProjection);
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection,uint32_t textureHandle);

	void SetEnableLighting(int32_t enableLigthing) {materialData_->enableLighting = enableLigthing;};
	void SetShiniess(float shininess) { materialData_->shininess = shininess; };
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	//Microsoft::WRL::ComPtr<ID3D12Resource> transformResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	VertexData* vertexData_ = nullptr;
	//uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	CameraForGpu* cameraData_ = nullptr;
	//TransformationMatrix* transformationMatrixData = nullptr;
	WorldTransform worldTransform;
	Vector2 position_;
	Vector2 size_;
	Vector4 color_;
	uint32_t textureHandle_;

	Matrix4x4 uvTransform_;

	Matrix4x4 wvp_;

	D3D12_RESOURCE_DESC resourceDesc_;

	size_t vertexNum = 0;
	std::vector<Model::MeshData*> meshs_;
};

