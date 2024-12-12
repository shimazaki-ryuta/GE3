#pragma once
#include "DirectXCommon.h"
#include "ModelStruct.h"
#include <d3d12.h>
#include <wrl.h>

//マテリアル

class Material {

public:

	// デバイス
	static ID3D12Device* sDevice;
	static void SetDevice(ID3D12Device* device);

	//初期化
	void Initialize();
	//ライティングするか(0:しない 1:一部 2: 完全 )
	void SetEnableLighting(int32_t enableLigthing) { materialData_->enableLighting = enableLigthing; };
	//鏡面反射強度
	void SetShiniess(float shininess) { materialData_->shininess = shininess; };
	//環境マッピング強度
	void SetEnvironmentCoefficient(float scale) { materialData_->environmentCoefficient = scale; };
	//アウトライン色
	void SetOutLineColor(const Vector4& color) { outlineData_->color = color; };
	//アウトライン太さ
	void SetOutLineWidth(const Vector3& wid) { outlineData_->scale = MakeScaleMatrix(wid); };
	//シェーディングタイプ 0:通常 1:2値化 
	void SetShadingType(int32_t num) { materialData_->shadingType = num; };
	//発光強度
	void SetGrowStrength(float strength) { materialData_->growStrength = strength; };

	//Getter
	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); };
	ID3D12Resource* GetOutlineResource() { return outlineResource_.Get(); };

	MaterialParamater paramater_;
	OutLineData outline_;

	//CPUパラメータをGPU側に反映
	void ApplyParamater();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	MaterialParamater* materialData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> outlineResource_;
	OutLineData* outlineData_ = nullptr;

};