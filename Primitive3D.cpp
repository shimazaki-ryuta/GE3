#include "Primitive3D.h"
#include "ShaderCompiler.h"
#include "TextureManager.h"
#include "MatrixFunction.h"

#define _USE_MATH_DEFINES
#include <math.h>

ID3D12Device* Primitive3D::sDevice = nullptr;
UINT Primitive3D::sDescriptorHandleIncrementSize;
ID3D12GraphicsCommandList* Primitive3D::sCommandList = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Primitive3D::sRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Primitive3D::sPipelineState;

void Primitive3D::StaticInitialize(
	ID3D12Device* device, int window_width, int window_height, const std::wstring& directoryPath)
{
	sDevice = device;
	sDescriptorHandleIncrementSize =
		sDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	//dxCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//includeに対応するための設定を行う
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	//Shaderのコンパイル
	IDxcBlob* vertexShaderBlob = CompileShader(L"Resources/Shaders/Primitive3DVS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"Resources/Shaders/Primitive3DPS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};//0->ps用materia,1->vs用,2->texture用
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);



	//シリアライズしてバイナリ化
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	//ID3D12RootSignature* rootSignature = nullptr;
	hr = sDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignature));
	assert(SUCCEEDED(hr));

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendDtateの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = sRootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジのタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;



	//実際に生成
	//ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = sDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineState));
	assert(SUCCEEDED(hr));
}

void Primitive3D::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Primitive3D::sCommandList == nullptr);

	// コマンドリストをセット
	sCommandList = commandList;

	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineState.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Primitive3D::PostDraw() {
	// コマンドリストを解除
	Primitive3D::sCommandList = nullptr;
}

Primitive3D* Primitive3D::Create(Vector2 position, Vector2 size, Vector4 color)
{
	Primitive3D* primitive3D = new Primitive3D();

	primitive3D->position_ = position;
	primitive3D->size_ = size;
	primitive3D->color_ = color;

	primitive3D->Initialize();

	return primitive3D;
}

void Primitive3D::Initialize()
{
	const uint32_t kSubdivision = 16;
	const uint32_t vertexCount = kSubdivision * kSubdivision * 6;

	vertNum = vertexCount;

	//resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);

	//Primitive3D用のリソースを作る
	vertexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexData) * vertNum);
	//頂点バッファビューを作る
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewPrimitive3D{};
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertNum;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);
	const float kLatEvery = float(M_PI) / float(kSubdivision);
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			float u = float(lonIndex) / float(kSubdivision);
			float v = 1.0f - float(latIndex + 1.0f) / float(kSubdivision);

			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			Vector4 a, b, c, d;
			a = { cosf(lat) * cosf(lon)			  ,sinf(lat)            ,cosf(lat) * sin(lon),1.0f };
			b = { cosf(lat + kLatEvery) * cosf(lon),sinf(lat + kLatEvery),cosf(lat + kLatEvery) * sin(lon),1.0f };
			c = { cosf(lat) * cosf(lon + kLonEvery),sinf(lat)            , cosf(lat) * sin(lon + kLonEvery),1.0f };
			d = { cosf(lat + kLatEvery) * cosf(lon + kLonEvery),sinf(lat + kLatEvery),cosf(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			//1枚目
			vertexData[start].position = a;
			vertexData[start + 1].position = b;
			vertexData[start + 2].position = c;
			
			//2枚目
			vertexData[start + 3].position = b;
			vertexData[start + 4].position = d;
			vertexData[start + 5].position = c;
		}
	}
	//マテリアル用のリソースを作成
	materialResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };


	transformResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(TransformationMatrix));

	transformResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataPrimitive3D));
	transformationMatrixDataPrimitive3D->WVP = MakeIdentity4x4();
	//transformationMatrixDataSprite->World = MakeIdentity4x4();

}

void Primitive3D::Draw(const Matrix4x4& wvp, const Vector4& color)
{
	Matrix4x4 worldMatrixPrimitive3D = MakeAffineMatrix(Vector3{ 0,0,0 }, Vector3{ 0,0,0 }, Vector3{ position_.x,position_.y,0.0f });
	Matrix4x4 viewMatrixPrimitive3D = MakeIdentity4x4();
	Matrix4x4 projectionMatrixPrimitive3D = MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixPrimitive3D = Multiply(worldMatrixPrimitive3D, Multiply(viewMatrixPrimitive3D, projectionMatrixPrimitive3D));
	transformationMatrixDataPrimitive3D->WVP = wvp;
	materialData_->color = color;


	sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle_);

	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
	sCommandList->DrawInstanced(vertNum, 1, 0, 0);
}
/*
void Primitive3D::DrawSphere(const Matrix4x4& wvp, const Vector4& color)
{

	Matrix4x4 worldMatrixPrimitive3D = MakeAffineMatrix(Vector3{ 0,0,0 }, Vector3{ 0,0,0 }, Vector3{ position_.x,position_.y,0.0f });
	Matrix4x4 viewMatrixPrimitive3D = MakeIdentity4x4();
	Matrix4x4 projectionMatrixPrimitive3D = MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixPrimitive3D = Multiply(worldMatrixPrimitive3D, Multiply(viewMatrixPrimitive3D, projectionMatrixPrimitive3D));
	transformationMatrixDataPrimitive3D->WVP = wvp;

	sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle_);

	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetIndexBuffer(&indexBufferView_);
	sCommandList->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
	sCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
*/