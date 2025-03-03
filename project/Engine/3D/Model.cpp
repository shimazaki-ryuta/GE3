#include "Model.h"
#include "ShaderCompiler.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
#include "LoadModel.h"
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//std::shared_ptr<D3DResourceLeakChacker>Model::leakchecker;
ID3D12Device* Model::sDevice = nullptr;
UINT Model::sDescriptorHandleIncrementSize;
ID3D12GraphicsCommandList* Model::sCommandList = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::sRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sPipelineState;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::sRootSignatureOutLine;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sPipelineStateOutLine;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::sRootSignatureSkinning;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sPipelineStateSkinning;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::sRootSignatureSkinningOutLine;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sPipelineStateSkinningOutLine;
//D3DResourceLeakChacker* Model::leakchecker = D3DResourceLeakChacker::GetInstance();
//Model::leakchecker.reset(D3DResourceLeakChacker::GetInstance());

void Model::StaticInitialize(
	ID3D12Device* device)
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
	D3D12_ROOT_PARAMETER rootParameters[11] = {};
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

	//pointLightお試し
	//rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//rootParameters[5].Descriptor.ShaderRegister = 3;
	D3D12_DESCRIPTOR_RANGE descriptorRangePL[1] = {};
	descriptorRangePL[0].BaseShaderRegister = 2;
	descriptorRangePL[0].NumDescriptors = 1;
	descriptorRangePL[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangePL[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRangePL;
	rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangePL);


	//spotLightお試し
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[6].Descriptor.ShaderRegister = 4;

	D3D12_DESCRIPTOR_RANGE descriptorRange2[1] = {};
	descriptorRange2[0].BaseShaderRegister = 1;
	descriptorRange2[0].NumDescriptors = 1;
	descriptorRange2[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange2[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRange2;
	rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange2);

	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[8].Descriptor.ShaderRegister = 8;

	D3D12_DESCRIPTOR_RANGE descriptorRangeEn[1] = {};
	descriptorRangeEn[0].BaseShaderRegister = 3;
	descriptorRangeEn[0].NumDescriptors = 1;
	descriptorRangeEn[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeEn[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRangeEn;
	rootParameters[9].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeEn);

	D3D12_DESCRIPTOR_RANGE descriptorRangeDisolve[1] = {};
	descriptorRangeDisolve[0].BaseShaderRegister = 4;
	descriptorRangeDisolve[0].NumDescriptors = 1;
	descriptorRangeDisolve[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeDisolve[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[10].DescriptorTable.pDescriptorRanges = descriptorRangeDisolve;
	rootParameters[10].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeDisolve);

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[1].ShaderRegister = 1;
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
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

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
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
	//重心
	inputElementDescs[3].SemanticName = "CENTER";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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
	graphicsPipelineStateDesc.NumRenderTargets = 2;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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
	StaticInitializeOutLine(device);
	CreateRootSignatureSkinning();
	CreateRootSignatureSkinningOutLine();
}

void Model::CreateRootSignatureSkinning() {
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
	IDxcBlob* vertexShaderBlob = CompileShader(L"Resources/Shaders/SkinningObject3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"Resources/Shaders/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[12] = {};
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

	//pointLightお試し
	//rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//rootParameters[5].Descriptor.ShaderRegister = 3;
	D3D12_DESCRIPTOR_RANGE descriptorRangePL[1] = {};
	descriptorRangePL[0].BaseShaderRegister = 2;
	descriptorRangePL[0].NumDescriptors = 1;
	descriptorRangePL[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangePL[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRangePL;
	rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangePL);


	//spotLightお試し
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[6].Descriptor.ShaderRegister = 4;

	D3D12_DESCRIPTOR_RANGE descriptorRange2[1] = {};
	descriptorRange2[0].BaseShaderRegister = 1;
	descriptorRange2[0].NumDescriptors = 1;
	descriptorRange2[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange2[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRange2;
	rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange2);

	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[8].Descriptor.ShaderRegister = 8;

	D3D12_DESCRIPTOR_RANGE descriptorRange3[1] = {};
	descriptorRange3[0].BaseShaderRegister = 4;
	descriptorRange3[0].NumDescriptors = 1;
	descriptorRange3[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange3[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRange3;
	rootParameters[9].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange3);

	D3D12_DESCRIPTOR_RANGE descriptorRangeEn[1] = {};
	descriptorRangeEn[0].BaseShaderRegister = 3;
	descriptorRangeEn[0].NumDescriptors = 1;
	descriptorRangeEn[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeEn[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[10].DescriptorTable.pDescriptorRanges = descriptorRangeEn;
	rootParameters[10].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeEn);

	D3D12_DESCRIPTOR_RANGE descriptorRangeDisolve[1] = {};
	descriptorRangeDisolve[0].BaseShaderRegister = 4;
	descriptorRangeDisolve[0].NumDescriptors = 1;
	descriptorRangeDisolve[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeDisolve[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[11].DescriptorTable.pDescriptorRanges = descriptorRangeDisolve;
	rootParameters[11].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeDisolve);

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[1].ShaderRegister = 1;
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
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
	hr = sDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignatureSkinning));
	assert(SUCCEEDED(hr));

	std::array<D3D12_INPUT_ELEMENT_DESC,5> inputElementDescs = {};
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
	
	//
	inputElementDescs[3].SemanticName = "WEIGHT";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].InputSlot = 1;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[4].SemanticName = "INDEX";
	inputElementDescs[4].SemanticIndex = 0;
	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	inputElementDescs[4].InputSlot = 1;
	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs.data();
	inputLayoutDesc.NumElements = UINT(inputElementDescs.size());

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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = sRootSignatureSkinning.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 2;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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
	hr = sDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineStateSkinning));
	assert(SUCCEEDED(hr));
}

void Model::CreateRootSignatureSkinningOutLine() {
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
	IDxcBlob* vertexShaderBlob = CompileShader(L"Resources/Shaders/SkinningOutlineVS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"Resources/Shaders/outlinePS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//mat
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Descriptor.ShaderRegister = 1;

	//transform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	D3D12_DESCRIPTOR_RANGE descriptorRange3[1] = {};
	descriptorRange3[0].BaseShaderRegister = 4;
	descriptorRange3[0].NumDescriptors = 1;
	descriptorRange3[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange3[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange3;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange3);

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
	hr = sDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignatureSkinningOutLine));
	assert(SUCCEEDED(hr));

	std::array<D3D12_INPUT_ELEMENT_DESC, 5> inputElementDescs = {};
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

	//
	inputElementDescs[3].SemanticName = "WEIGHT";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].InputSlot = 1;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[4].SemanticName = "INDEX";
	inputElementDescs[4].SemanticIndex = 0;
	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	inputElementDescs[4].InputSlot = 1;
	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs.data();
	inputLayoutDesc.NumElements = UINT(inputElementDescs.size());

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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = sRootSignatureSkinningOutLine.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 2;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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
	hr = sDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineStateSkinningOutLine));
	assert(SUCCEEDED(hr));
}

void Model::StaticInitializeOutLine(
	ID3D12Device* device)
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
	IDxcBlob* vertexShaderBlob = CompileShader(L"Resources/Shaders/outlineVS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"Resources/Shaders/outlinePS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	//mat
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Descriptor.ShaderRegister = 1;

	//transform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	
	
	

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
	hr = sDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignatureOutLine));
	assert(SUCCEEDED(hr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
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
	//重心
	inputElementDescs[3].SemanticName = "CENTER";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = sRootSignatureOutLine.Get();
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
	hr = sDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineStateOutLine));
	assert(SUCCEEDED(hr));
}

void Model::Create(const  std::string& directoryPath, const std::string& filename)
{
	modelData_ = LoadModel::LoadModelFile(directoryPath, filename);

	//std::string filePath = directoryPath + "/" + filename;

	vertexNum = modelData_.vertexNum;
	textureHandle_ = modelData_.meshs.material.textureHandle;
	toonShadowTextureHandle_ = textureHandle_;
	perspectivTextureHandle_ = textureHandle_;
	disolveMaskTextureHandle_ = TextureManager::LoadTexture("noise0.png");
	//頂点リソース
	 vertexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexData) *vertexNum);

	//頂点バッファ
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertexNum);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.meshs.vertices.data(), sizeof(VertexData) * vertexNum);
	
	//インデックスリソース
	indexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(uint32_t) * modelData_.meshs.indices.size());
	
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = uint32_t(sizeof(uint32_t) * modelData_.meshs.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_,modelData_.meshs.indices.data(),sizeof(uint32_t)*modelData_.meshs.indices.size());
	modelData_.meshs.indicesNum = uint32_t(modelData_.meshs.indices.size());

	//マテリアル用のリソースを作成
	materialResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(MaterialParamater));
	//Material* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->disolveColor = Vector4{1.0f,1.0f,1.0f,1.0f};
	materialData_->enableLighting = 0;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 100.0f;
	materialData_->growStrength = 0;
	materialData_->environmentCoefficient = 0;
	materialData_->shadingType = 0;

	//ptrに内部のリソースセット
	materialResourcePtr_ = materialResource_.Get();

	//ライティング用のカメラリソース
	cameraResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(CameraForGpu));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	cameraData_->worldPosition = {0,0,0};

	//アウトライン用リソース
	outlineResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(OutLineData));
	//Material* materialData = nullptr;
	outlineResource_->Map(0, nullptr, reinterpret_cast<void**>(&outlineData_));
	outlineData_->color = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };
	outlineData_->scale = MakeScaleMatrix({1.05f,1.05f,1.05f});
	outlineResourcePtr_ = outlineResource_.Get();

	//ローカル変換行列
	localMatrixResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(Matrix4x4));
	localMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&localMatrixData_));
	*localMatrixData_ = MakeIdentity4x4();
	if (!modelData_.rootNode.name.empty()) {
		*localMatrixData_ = modelData_.rootNode.localMatrix;
	}
}

void Model::CreateTerrain(const  std::string& directoryPath, const std::string& filename)
{
	modelData_ = LoadModel::LoadObjFile(directoryPath, filename);

	//std::string filePath = directoryPath + "/" + filename;

	vertexNum = modelData_.vertexNum;
	textureHandle_ = modelData_.meshs.material.textureHandle;
	toonShadowTextureHandle_ = TextureManager::LoadTexture("toonShadow1.png"); perspectivTextureHandle_ = textureHandle_;
	disolveMaskTextureHandle_ = TextureManager::LoadTexture("noise0.png");

	//仮最大数
	//size_t vertMaxNum = 16384;

	//余剰頂点数
	size_t vertexAppendMax = 65536*12;

	//頂点リソース
	vertexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexData) * (vertexNum + vertexAppendMax));
	//一時的に最大数固定
	//vertexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexData) * (vertMaxNum));

	//頂点バッファ
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * (vertexNum + vertexAppendMax));
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.meshs.vertices.data(), sizeof(VertexData) * vertexNum);
	modelData_.meshs.indicesNum = uint32_t(vertexNum);
	//インデックスバッファーを同じだけ作る
	modelData_.meshs.indices.clear();
	for (uint32_t i = 0; i < vertexNum + vertexAppendMax;i++) {
		modelData_.meshs.indices.push_back(i);
	}

	//インデックスリソース
	indexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(uint32_t) * modelData_.meshs.indices.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = uint32_t(sizeof(uint32_t) * modelData_.meshs.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, modelData_.meshs.indices.data(), sizeof(uint32_t) * modelData_.meshs.indices.size());
	
	//マテリアル用のリソースを作成
	materialResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(MaterialParamater));
	//Material* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->disolveColor = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = 0;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 100.0f;
	materialData_->growStrength = 0;
	materialData_->environmentCoefficient = 0;
	materialData_->shadingType = 0;

	//ptrに内部のリソースセット
	materialResourcePtr_ = materialResource_.Get();

	//ライティング用のカメラリソース
	cameraResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(CameraForGpu));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	cameraData_->worldPosition = { 0,0,0 };

	//アウトライン用リソース
	outlineResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(OutLineData));
	//Material* materialData = nullptr;
	outlineResource_->Map(0, nullptr, reinterpret_cast<void**>(&outlineData_));
	outlineData_->color = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };
	outlineData_->scale = MakeScaleMatrix({ 1.05f,1.05f,1.05f });
	outlineResourcePtr_ = outlineResource_.Get();

	//ローカル変換行列
	localMatrixResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(Matrix4x4));
	localMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&localMatrixData_));
	*localMatrixData_ = MakeIdentity4x4();
	if (!modelData_.rootNode.name.empty()) {
		*localMatrixData_ = modelData_.rootNode.localMatrix;
	}
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
	//assert(Model::sCommandList == nullptr);

	// コマンドリストをセット
	sCommandList = commandList;

	// パイプラインステートの設定
	//sCommandList->SetPipelineState(sPipelineState.Get());

	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::PreDrawOutLine(ID3D12GraphicsCommandList* commandList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	//assert(Model::sCommandList == nullptr);

	// コマンドリストをセット
	sCommandList = commandList;

	// パイプラインステートの設定
	//sCommandList->SetPipelineState(sPipelineState.Get());

	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignatureOutLine.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::PostDraw() {
	// コマンドリストを解除
	Model::sCommandList = nullptr;
}

void Model::TransferBuffer() {
	//std::memcpy(vertexData_, modelData_.meshs.vertices.data(), sizeof(VertexData) * modelData_.meshs.vertices.size());
	std::memcpy(indexData_, modelData_.meshs.indices.data(), sizeof(uint32_t) * modelData_.meshs.indices.size());
}

void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection) {
	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineState.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//transformationMatrixData->WVP = worldTransform.matWorld_ * viewProjection.matView * viewProjection.matProjection;
	//transformationMatrixData->World = worldTransform.matWorld_;
	worldTransform.TransfarMatrix(viewProjection.matView * viewProjection.matProjection);
	cameraData_->worldPosition = viewProjection.translation_;
	sCommandList->SetGraphicsRootConstantBufferView(0, materialResourcePtr_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());
	//localMatrix用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(8, localMatrixResource_->GetGPUVirtualAddress());
	//Lighting用のリソースの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	//camera用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

	//sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(7, toonShadowTextureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(9, perspectivTextureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(10, disolveMaskTextureHandle_);
	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	sCommandList->IASetIndexBuffer(&indexBufferView_);

	//sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	//sCommandList->DrawInstanced(UINT(vertexNum), 1, 0, 0);
	sCommandList->DrawIndexedInstanced(UINT(modelData_.meshs.indicesNum), 1, 0, 0, 0);
}

void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHandle) {
	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineState.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//transformationMatrixData->WVP = worldTransform.matWorld_ * viewProjection.matView * viewProjection.matProjection;
	//transformationMatrixData->World = worldTransform.matWorld_;
	worldTransform.TransfarMatrix(viewProjection.matView * viewProjection.matProjection);
	cameraData_->worldPosition = viewProjection.translation_;
	sCommandList->SetGraphicsRootConstantBufferView(0, materialResourcePtr_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());
	//localMatrix用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(8, localMatrixResource_->GetGPUVirtualAddress());
	//Lighting用のリソースの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	//camera用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

	//sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(7, toonShadowTextureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(9, perspectivTextureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(10, disolveMaskTextureHandle_);
	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	sCommandList->IASetIndexBuffer(&indexBufferView_);

	//sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	sCommandList->DrawIndexedInstanced(UINT(modelData_.meshs.indices.size()), 1, 0, 0,0);

}

void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, SkinCluster& skinCluster) {
	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineStateSkinning.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignatureSkinning.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//transformationMatrixData->WVP = worldTransform.matWorld_ * viewProjection.matView * viewProjection.matProjection;
	//transformationMatrixData->World = worldTransform.matWorld_;
	worldTransform.TransfarMatrix(viewProjection.matView * viewProjection.matProjection);
	cameraData_->worldPosition = viewProjection.translation_;
	sCommandList->SetGraphicsRootConstantBufferView(0, materialResourcePtr_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());
	//localMatrix用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(8, localMatrixResource_->GetGPUVirtualAddress());
	//Lighting用のリソースの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	//camera用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

	//sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(7, toonShadowTextureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(10, perspectivTextureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(11, disolveMaskTextureHandle_);
	sCommandList->SetGraphicsRootDescriptorTable(9,skinCluster.palleteSrvHandle.second);

	//vb
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		vertexBufferView_,
		skinCluster.influenceBufferView
	};
	sCommandList->IASetVertexBuffers(0, 2, vbvs);
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	sCommandList->IASetIndexBuffer(&indexBufferView_);

	//sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	sCommandList->DrawIndexedInstanced(UINT(modelData_.meshs.indices.size()), 1, 0, 0, 0);

}

void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection,ID3D12Resource* animationMatrixResource) {
	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineState.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//transformationMatrixData->WVP = worldTransform.matWorld_ * viewProjection.matView * viewProjection.matProjection;
	//transformationMatrixData->World = worldTransform.matWorld_;
	worldTransform.TransfarMatrix(viewProjection.matView * viewProjection.matProjection);
	cameraData_->worldPosition = viewProjection.translation_;
	sCommandList->SetGraphicsRootConstantBufferView(0, materialResourcePtr_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());
	//localMatrix用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(8, animationMatrixResource->GetGPUVirtualAddress());
	//Lighting用のリソースの場所を設定
	//sCommandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
	//camera用のリソース
	sCommandList->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

	//sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(7, toonShadowTextureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(9, perspectivTextureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(10, disolveMaskTextureHandle_);
	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	sCommandList->IASetIndexBuffer(&indexBufferView_);

	//sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	sCommandList->DrawIndexedInstanced(UINT(modelData_.meshs.indices.size()), 1, 0, 0,0);

}

void Model::DrawOutLine(WorldTransform& worldTransform, const ViewProjection& viewProjection) {
	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineStateOutLine.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignatureOutLine.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	worldTransform.TransfarMatrix(viewProjection.matView * viewProjection.matProjection);

	sCommandList->SetGraphicsRootConstantBufferView(0, outlineResourcePtr_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());
	
	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	sCommandList->IASetIndexBuffer(&indexBufferView_);

	//sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	//sCommandList->DrawInstanced(UINT(vertexNum), 1, 0, 0);
	sCommandList->DrawIndexedInstanced(UINT(modelData_.meshs.indices.size()), 1, 0, 0, 0);
}

void Model::DrawOutLine(WorldTransform& worldTransform, const ViewProjection& viewProjection, SkinCluster& skinCluster) {
	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineStateSkinningOutLine.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignatureSkinningOutLine.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	worldTransform.TransfarMatrix(viewProjection.matView * viewProjection.matProjection);

	sCommandList->SetGraphicsRootConstantBufferView(0, outlineResourcePtr_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(1, worldTransform.transformResource_->GetGPUVirtualAddress());

	sCommandList->SetGraphicsRootDescriptorTable(2, skinCluster.palleteSrvHandle.second);

	//vb
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		vertexBufferView_,
		skinCluster.influenceBufferView
	};
	sCommandList->IASetVertexBuffers(0, 2, vbvs);
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	sCommandList->IASetIndexBuffer(&indexBufferView_);

	//sCommandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	//sCommandList->DrawInstanced(UINT(vertexNum), 1, 0, 0);
	sCommandList->DrawIndexedInstanced(UINT(modelData_.meshs.indices.size()), 1, 0, 0, 0);
}
