#include "Particle.h"
#include "ShaderCompiler.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "GetDescriptorHandle.h"
#include "LoadModel.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <numbers>
#include <string>
#include <wrl.h>
#include "DeltaTime.h"
#include "VectorFunction.h"
#include "RandomEngine.h"
//std::shared_ptr<D3DResourceLeakChacker>Particle::leakchecker;
ID3D12Device* Particle::sDevice = nullptr;
UINT Particle::sDescriptorHandleIncrementSize;
ID3D12GraphicsCommandList* Particle::sCommandList = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Particle::sRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Particle::sPipelineState;
ID3D12DescriptorHeap* Particle::srvDescriptorHeap_ = nullptr;
size_t Particle::kSrvStructuredBufferUseBegin;

void Particle::StaticInitialize(
	ID3D12Device* device, ID3D12DescriptorHeap* descriptorHeap, const std::wstring& directoryPath)
{
	//leakchecker.reset(D3DResourceLeakChacker::GetInstance());
	sDevice = device;
	srvDescriptorHeap_ = descriptorHeap;
	sDescriptorHandleIncrementSize =
		sDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	kSrvStructuredBufferUseBegin = TextureManager::kSrvTextureUseEnd + 1;

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
	IDxcBlob* vertexShaderBlob = CompileShader(L"Resources/Shaders/Particle.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"Resources/Shaders/Particle.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	//Instancing
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	//texture
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

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
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
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//実際に生成
	//ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = sDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineState));
	assert(SUCCEEDED(hr));
}


void Particle::Initialize(uint32_t numInstance)
{
	numInstanceMax_ = numInstance;
	numInstance_ = numInstanceMax_;
	//textureHandle_ = textureHandle;
	//Sprite用のリソースを作る
	std::string directory = "Resources/Plane";
	std::string filename = "Plane";
	filename = filename + ".obj";
	LoadModel::ModelData modelData = LoadModel::LoadObjFile(directory, filename);
	vertexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexData) * 6);
	textureHandle_ = modelData.meshs.material.textureHandle;
	//頂点バッファビューを作る
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));

	/*//1枚目
	//左下
	vertexDataSprite_[0].position = { -1.0f,-1.0f,0.0f,1.0f };
	vertexDataSprite_[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite_[0].normal = {0.0f,0.0f,1.0f};
	//右下
	vertexDataSprite_[1].position = { 1.0f,-1,0.0f,1.0f };
	vertexDataSprite_[1].texcoord = { 1.0f,1.0f };
	vertexDataSprite_[1].normal = { 0.0f,0.0f,1.0f };
	//左上
	vertexDataSprite_[2].position = { -1.0f,1.0f,0.0f,1.0f };
	vertexDataSprite_[2].texcoord = { 0.0f,0.0f };
	vertexDataSprite_[2].normal = { 0.0f,0.0f,1.0f };
	//右上
	vertexDataSprite_[3].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexDataSprite_[3].texcoord = { 1.0f,0.0f };
	vertexDataSprite_[3].normal = { 0.0f,0.0f,1.0f };*/
	std::memcpy(vertexDataSprite_, modelData.meshs.vertices.data(), sizeof(VertexData) * 6);

	//インデックス
	indexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(uint32_t) * 6);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 2;
	indexData_[5] = 3;

	instancingResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(ParticleForGPU) * numInstance_);

	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	for (uint32_t index = 0; index < numInstance_; ++index) {
		instancingData[index].WVP = MakeIdentity4x4();
		instancingData[index].World = MakeIdentity4x4();
		instancingData[index].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	uvTransform_ = MakeIdentity4x4();

	//スプライト用のマテリアルリソースを作成
	materialResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->uvTransform = uvTransform_;

	//srvの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = numInstance_;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	srvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap_, sDescriptorHandleIncrementSize, uint32_t(kSrvStructuredBufferUseBegin));
	srvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap_, sDescriptorHandleIncrementSize, uint32_t(kSrvStructuredBufferUseBegin));

	sDevice->CreateShaderResourceView(instancingResource_.Get(), &srvDesc, srvHandleCPU);
	kSrvStructuredBufferUseBegin++;
	/*
	//transform(仮)
	for (uint32_t index = 0; index < numInstance_; ++index) {
		struct Transform transform;
		transform.scale = {1.0f,1.0f,1.0f};
		transform.rotate = {0.0f,std::numbers::pi_v<float>,0.0f};
		transform.translate = {RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f), RandomEngine::GetRandom(-1.0f,1.0f) };
		//transforms.push_back(transform);
		//ParticleData particle;
		//particle.transform = transform;
		Vector3 velocity = { RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f), RandomEngine::GetRandom(-1.0f,1.0f) };
		Vector4 color = { RandomEngine::GetRandom(0.0f,1.0f),RandomEngine::GetRandom(0.0f,1.0f), RandomEngine::GetRandom(0.0f,1.0f),1.0f };
		float lifeTime = RandomEngine::GetRandom(1.0f, 3.0f);
		particleData_.push_back(ParticleData{ transform,velocity,color,lifeTime,0 });
	}*/
}

void Particle::MakeNewParticle(const Vector3& translate) {
	struct Transform transform;
	transform.scale = { 1.0f,1.0f,1.0f };
	transform.rotate = { 0.0f,0.0f,0.0f };
	transform.translate = translate;
	//transforms.push_back(transform);
	//ParticleData particle;
	//particle.transform = transform;
	Vector3 velocity = { RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f), RandomEngine::GetRandom(-1.0f,1.0f) };
	Vector4 color = { RandomEngine::GetRandom(0.0f,1.0f),RandomEngine::GetRandom(0.0f,1.0f), RandomEngine::GetRandom(0.0f,1.0f),1.0f };
	float lifeTime = RandomEngine::GetRandom(1.0f, 3.0f);
	particleData_.push_back(ParticleData{ transform,velocity,color,lifeTime,0 });
}

void Particle::MakeNewParticle(const ParticleData& particleData) {
	particleData_.push_back(particleData);
}

void Particle::Emit(const Emitter& emitter) {
	ParticleData particleData;
	for (uint32_t count = 0; count < emitter.count; count++) {
		particleData.transform.scale = { 1.0f,1.0f,1.0f };
		particleData.transform.rotate = { 0.0f,0.0f,0.0f };
		particleData.transform.translate = emitter.transform.translate + Vector3{ RandomEngine::GetRandom(-1.0f, 1.0f), RandomEngine::GetRandom(-1.0f, 1.0f), RandomEngine::GetRandom(-1.0f, 1.0f) };
		particleData.velocity = { RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f), RandomEngine::GetRandom(-1.0f,1.0f) };
		particleData.color = { RandomEngine::GetRandom(0.0f,1.0f),RandomEngine::GetRandom(0.0f,1.0f), RandomEngine::GetRandom(0.0f,1.0f),1.0f };
		particleData.lifeTime = RandomEngine::GetRandom(1.0f, 3.0f);
		particleData.currentTime = 0;
		MakeNewParticle(particleData);
	}
}

Particle* Particle::Create(uint32_t textureHandle, uint32_t numInstance)
{
	Particle* particle = new Particle();
	particle->Initialize(numInstance);
	particle->SetTextureHandle(textureHandle);
	return particle;
}

Particle* Particle::Create(uint32_t numInstance)
{
	Particle* particle = new Particle();
	particle->Initialize(numInstance);
	return particle;
}

void Particle::Updade() {
	numInstance_ = 0;
	for (std::vector<ParticleData>::iterator particleIterator = particleData_.begin(); particleIterator != particleData_.end();) {
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
			particleIterator = particleData_.erase(particleIterator);
			continue;
		}

		float deltaTime = 1.0f / 60.0f;
		(*particleIterator).transform.translate += deltaTime * (*particleIterator).velocity;
		(*particleIterator).currentTime += deltaTime;
		float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
		(*particleIterator).color.w = alpha;
		numInstance_++;
		++particleIterator;
	}
	numInstance_ = std::clamp(numInstance_, uint32_t(0), numInstanceMax_);
}

void Particle::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Particle::sCommandList == nullptr);

	// コマンドリストをセット
	sCommandList = commandList;

	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineState.Get());

	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Particle::PostDraw() {
	// コマンドリストを解除
	Particle::sCommandList = nullptr;
}

void Particle::Draw(const ViewProjection& viewProjection) {

	Matrix4x4 billBoardMatrix = MakeIdentity4x4();
	if (isBillboard_) {
		billBoardMatrix = MakeRotateMatrix({ 0.0f,std::numbers::pi_v<float>,0.0f }) * Inverse(viewProjection.matView);
		billBoardMatrix.m[3][0] = 0;
		billBoardMatrix.m[3][1] = 0;
		billBoardMatrix.m[3][2] = 0;
		//billBoardMatrix = Inverse(billBoardMatrix);
	}
	Matrix4x4 viewProjectionMatrix = viewProjection.matView * viewProjection.matProjection;
	for (uint32_t index = 0; index < numInstance_; index++) {
		//Matrix4x4 world = MakeAffineMatrix(particleData_[index].transform.scale, particleData_[index].transform.rotate, particleData_[index].transform.translate) * billBoardMatrix;
		Matrix4x4 world = MakeScaleMatrix(particleData_[index].transform.scale) * MakeRotateMatrix(particleData_[index].transform.rotate) * billBoardMatrix * MakeTranslateMatrix(particleData_[index].transform.translate);
		Matrix4x4 worldViewProjection = world * viewProjectionMatrix;
		instancingData[index].WVP = worldViewProjection;
		instancingData[index].World = world;
		instancingData[index].Color = particleData_[index].color;
	}

	sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());
	sCommandList->SetGraphicsRootDescriptorTable(1, srvHandleGPU);

	//Lighting用のリソースの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());


	//sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle_);

	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//sCommandList->IASetIndexBuffer(&indexBufferView_);
	sCommandList->DrawInstanced(6, numInstance_, 0, 0);

}