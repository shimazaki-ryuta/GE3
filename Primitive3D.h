#pragma once
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix.h"

#include <d3d12.h>
#include <string>
#include <wrl.h>

class Primitive3D
{
public:
	struct VertexData
	{
		Vector4 position;
	};
	struct Material
	{
		Vector4 color;
	};

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	// 頂点数
	//static const int kVertNum = 4;
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
	void Draw(const Matrix4x4& wvp, const Vector4& color);
	static void PostDraw();

	static Primitive3D* Create(Vector2 position, Vector2 size, Vector4 color = { 1, 1, 1, 1 });

	void Initialize();

	inline void SetPosition(const Vector2& pos) { position_ = pos; };

	inline void SetWVP(const Matrix4x4& wvp) { wvp_ = wvp; };

	//void DrawSphere(const Matrix4x4& wvp,const Vector4& color);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	VertexData* vertexDataPrimitive3D_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* transformationMatrixDataPrimitive3D = nullptr;
	Vector2 position_;
	Vector2 size_;
	Vector4 color_;
	uint32_t textureHandle_;

	Matrix4x4 wvp_;

	D3D12_RESOURCE_DESC resourceDesc_;

	//球

	//頂点数
	int vertNum = 0;

};

