#include "Model.h"
#include "ShaderCompiler.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "LoadModel.h"
#include <fstream>
#include <sstream>
//std::shared_ptr<D3DResourceLeakChacker>Model::leakchecker;
ID3D12Device* Model::sDevice = nullptr;
UINT Model::sDescriptorHandleIncrementSize;
ID3D12GraphicsCommandList* Model::sCommandList = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::sRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sPipelineState;
//D3DResourceLeakChacker* Model::leakchecker = D3DResourceLeakChacker::GetInstance();
//Model::leakchecker.reset(D3DResourceLeakChacker::GetInstance());

void Model::StaticInitialize(
	ID3D12Device* device, int window_width, int window_height, const std::wstring& directoryPath)
{
	//leakchecker.reset(D3DResourceLeakChacker::GetInstance());
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
	IDxcBlob* vertexShaderBlob = CompileShader(L"Resources/Shaders/Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"Resources/Shaders/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
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

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;

	//camera
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 2;

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

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//02_04追加分
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//法線
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendDtateの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	/*blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;*/
	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
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

void Model::Create(const  std::string& directoryPath, const std::string& filename)
{
	LoadModel::ModelData modelData = LoadModel::LoadObjFile(directoryPath, filename);
	vertexNum = modelData.vertexNum;
	textureHandle_ = modelData.meshs.material.textureHandle;
	//頂点リソース
	 vertexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexData) *vertexNum);

	//頂点バッファ
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertexNum);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData.meshs.vertices.data(), sizeof(VertexData) * vertexNum);
	
	//マテリアル用のリソースを作成
	materialResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(Material));
	//Material* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = 2;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 100.0f;
	//ライティング用のカメラリソース
	cameraResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(CameraForGpu));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	cameraData_->worldPosition = {0,0,0};
}

Model* Model::CreateFromOBJ(const  std::string& filename)
{
	std::string directoryPath = "Resources/" + filename;
	Model* model = new Model();
	model->Create(directoryPath,filename + ".obj");
	return model;
}

void Model::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Model::sCommandList == nullptr);

	// コマンドリストをセット
	sCommandList = commandList;

	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineState.Get());

	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::PostDraw() {
	// コマンドリストを解除
	Model::sCommandList = nullptr;
}


void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection) {

	//transformationMatrixData->WVP = worldTransform.matWorld_ * viewProjection.matView * viewProjection.matProjection;
	//transformationMatrixData->World = worldTransform.matWorld_;
	worldTransform.TransfarMatrix(viewProjection.matView * viewProjection.matProjection);

	cameraData_->worldPosition = viewProjection.translation_;

	sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());
	//Lighting用のリソースの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	//camera用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());


	//sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle_);

	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	sCommandList->DrawInstanced(UINT(vertexNum), 1, 0, 0);

}

void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHandle) {

	//transformationMatrixData->WVP = worldTransform.matWorld_ * viewProjection.matView * viewProjection.matProjection;
	//transformationMatrixData->World = worldTransform.matWorld_;
	worldTransform.TransfarMatrix(viewProjection.matView * viewProjection.matProjection);
	cameraData_->worldPosition = viewProjection.translation_;
	sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());
	//Lighting用のリソースの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	//camera用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

	//sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle);

	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	sCommandList->DrawInstanced(UINT(vertexNum), 1, 0, 0);

}
