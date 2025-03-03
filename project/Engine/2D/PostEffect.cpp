#include "PostEffect.h"
#include "ShaderCompiler.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
//std::shared_ptr<D3DResourceLeakChacker>PostEffect::leakchecker;
ID3D12Device* PostEffect::sDevice = nullptr;
UINT PostEffect::sDescriptorHandleIncrementSize;
ID3D12GraphicsCommandList* PostEffect::sCommandList = nullptr;
//Microsoft::WRL::ComPtr<ID3D12RootSignature> PostEffect::sRootSignature;
//Microsoft::WRL::ComPtr<ID3D12PipelineState> PostEffect::sPipelineState;
//std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(PostEffect::BlendMode::CountofBlendMode)> PostEffect::sPipelineStates;

void PostEffect::SetDevice(ID3D12Device* device) {
	sDevice = device;
}

void PostEffect::StaticInitialize(const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath)
{
	//leakchecker.reset(D3DResourceLeakChacker::GetInstance());
	//sDevice = device;
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
	IDxcBlob* vertexShaderBlob = CompileShader(vertexShaderPath, L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(pixelShaderPath, L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};//0->ps用materia,1->vs用,2->texture用
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

	//第二テクスチャ用
	D3D12_DESCRIPTOR_RANGE descriptorRange2[1] = {};
	descriptorRange2[0].BaseShaderRegister = 1;
	descriptorRange2[0].NumDescriptors = 1;
	descriptorRange2[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange2[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange2;
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange2);


	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
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

	
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;

	//BlendDtateの設定
	D3D12_BLEND_DESC blendDesc[size_t(BlendMode::CountofBlendMode)]{};
	//共通
	for (int index = size_t(BlendMode::Normal); index < size_t(BlendMode::CountofBlendMode); index++)
	{
		blendDesc[index].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc[index].RenderTarget[0].BlendEnable = TRUE;
		blendDesc[index].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc[index].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc[index].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc[index].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc[index].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc[index].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	}
	//blendmode固有
	blendDesc[size_t(BlendMode::None)].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blendDesc[size_t(BlendMode::Normal)].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc[size_t(BlendMode::Normal)].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc[size_t(BlendMode::Normal)].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc[size_t(BlendMode::Add)].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc[size_t(BlendMode::Add)].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc[size_t(BlendMode::Add)].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc[size_t(BlendMode::Subtract)].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc[size_t(BlendMode::Subtract)].RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc[size_t(BlendMode::Subtract)].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc[size_t(BlendMode::Multiply)].RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc[size_t(BlendMode::Multiply)].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc[size_t(BlendMode::Multiply)].RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

	blendDesc[size_t(BlendMode::Screen)].RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc[size_t(BlendMode::Screen)].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc[size_t(BlendMode::Screen)].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//PSOの生成
	for (int index = size_t(BlendMode::None); index < size_t(BlendMode::CountofBlendMode); index++)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = sRootSignature.Get();
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
		graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.BlendState = blendDesc[index];
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
		//書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//利用するトポロジのタイプ、三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//どのように画面に色を打ち込むかの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		//実際に生成
		//ID3D12PipelineState* graphicsPipelineState = nullptr;
		hr = sDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineStates[index]));
		assert(SUCCEEDED(hr));
	}
}

void PostEffect::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	//assert(PostEffect::sCommandList == nullptr);

	// コマンドリストをセット
	sCommandList = commandList;

	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void PostEffect::PostDraw() {
	// コマンドリストを解除
	PostEffect::sCommandList = nullptr;
}

PostEffect* PostEffect::Create(Vector2 position, Vector2 size, Vector4 color)
{
	PostEffect* postEffect = new PostEffect();

	postEffect->position_ = position;
	postEffect->size_ = size;
	postEffect->color_ = color;
	postEffect->anchorPoint_ = { 0.5f,0.5f };
	postEffect->blendMode_ = BlendMode::Screen;
	postEffect->rotate_ = 0;
	//postEffect->Initialize();

	return postEffect;
}

void PostEffect::Initialize(const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath)
{
//	resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);

	StaticInitialize(vertexShaderPath,pixelShaderPath);

	//Sprite用のリソースを作る
	vertexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexData) * 4);
	//頂点バッファビューを作る
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));
	//1枚目
	vertexDataSprite_[0].position = { -1.0f,1.0f,0.0f,1.0f };
	vertexDataSprite_[0].texcoord = { 0.0f,0.0f };
	vertexDataSprite_[1].position = { 3.0f, 1.0f,0.0f,1.0f };
	vertexDataSprite_[1].texcoord = { 2.0f,0.0f };
	vertexDataSprite_[2].position = { -1.0f,-3.0f,0.0f,1.0f };
	vertexDataSprite_[2].texcoord = { 0.0f,2.0f };
	//vertexDataSprite_[3].position = { +size_.x * (1.0f - anchorPoint_.x), +size_.y * (1.0f - anchorPoint_.y),0.0f,1.0f };
	//vertexDataSprite_[3].texcoord = { 1.0f,1.0f };

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

	transformResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(TransformationMatrix));

	transformResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	transformationMatrixDataSprite->World = MakeIdentity4x4();

	uvTransform_ = MakeIdentity4x4();

	//スプライト用のマテリアルリソースを作成
	materialResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->uvTransform = uvTransform_;

}

void PostEffect::SetRange(const Vector2& leftTop, const Vector2& rightDown) {
	struct SRT uvTransform = { {(rightDown.x - leftTop.x) / size_.x,(rightDown.y - leftTop.y) / size_.y ,1},{0,0,0},{leftTop.x / size_.x,leftTop.y / size_.y,0} };
	uvTransform_ = MakeAffineMatrix(uvTransform.scale, uvTransform.rotate, uvTransform.translate);
}

void PostEffect::Draw(ID3D12DescriptorHeap* srvDescriptorHeap, D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU, D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU2)
{
	sCommandList->SetPipelineState(sPipelineStates[size_t(blendMode_)].Get());

	//1枚目
	/*vertexDataSprite_[0].position = {-size_.x * anchorPoint_.x, -size_.y * anchorPoint_.y,0.0f,1.0f};
	vertexDataSprite_[0].texcoord = { 0.0f,0.0f };
	vertexDataSprite_[1].position = { +size_.x * (1.0f - anchorPoint_.x), -size_.y * anchorPoint_.y,0.0f,1.0f };
	vertexDataSprite_[1].texcoord = { 1.0f,0.0f };
	vertexDataSprite_[2].position = { -size_.x * anchorPoint_.x, +size_.y * (1.0f - anchorPoint_.y),0.0f,1.0f };
	vertexDataSprite_[2].texcoord = { 0.0f,1.0f };
	vertexDataSprite_[3].position = { +size_.x * (1.0f - anchorPoint_.x), +size_.y * (1.0f - anchorPoint_.y),0.0f,1.0f };
	vertexDataSprite_[3].texcoord = { 1.0f,1.0f };
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, Vector3{ 0,0,rotate_ }, Vector3{ position_.x,position_.y,0.0f });
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 1.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;*/
	materialData_->uvTransform = uvTransform_;

	sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
	sCommandList->SetDescriptorHeaps(1, descriptorHeaps);

	//SRVのDescriptorTableの先頭を設定。
	sCommandList->SetGraphicsRootDescriptorTable(2, srvHandleGPU);
	sCommandList->SetGraphicsRootDescriptorTable(3, srvHandleGPU2);

	//sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetIndexBuffer(&indexBufferView_);
	//sCommandList->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
	sCommandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}
