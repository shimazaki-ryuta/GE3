#include "Material.h"

ID3D12Device* Material::sDevice = nullptr;

void Material::SetDevice(ID3D12Device* device) {
	sDevice = device;
}

void Material::Initialize() {
	materialResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(MaterialParamater));
	//Material* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = 0;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 100.0f;
	materialData_->growStrength = 0;
	materialData_->environmentCoefficient = 0;
	materialData_->shadingType = 0;
	materialData_->disolveThreshold = 0.0f;

	outlineResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(OutLineData));
	//Material* materialData = nullptr;
	outlineResource_->Map(0, nullptr, reinterpret_cast<void**>(&outlineData_));
	outlineData_->color = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };
	outlineData_->scale = MakeScaleMatrix({ 0.05f,0.05f,0.05f });

	paramater_ = *materialData_;
	outline_ = *outlineData_;
}

void Material::ApplyParamater() {
	*materialData_  = paramater_;
	*outlineData_ = outline_;
}