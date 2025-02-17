#include "BaseCharacter.h"

void BaseCharacter::Initialize(const std::vector<HierarchicalAnimation>& models)
{
	models_ = models;
	worldTransform_.Initialize();
}

void BaseCharacter::Update() 
{
	worldTransform_.UpdateMatrix();
}

void BaseCharacter::Draw(const ViewProjection& viewProjection) 
{
	//内包する全モデルを描画する
	for (HierarchicalAnimation& model : models_) {
		model.model_->Draw(worldTransform_, viewProjection);
	}
}

void BaseCharacter::SetShadowTexture(uint32_t handle) {
	//内包する全モデルに影用テクスチャをセットする
	for (HierarchicalAnimation& model : models_) {
		model.model_->SetEnableLighting(2);
		model.model_->SetToonShadowTextureHandle(handle);
		model.model_->SetShadingType(1);
		model.model_->SetGrowStrength(0.1f);
	}
}

void BaseCharacter::SetOutLineData(float width, const Vector4& color) {
	for (HierarchicalAnimation& model : models_) {
		model.model_->SetOutLineWidth({width,width ,width });
		model.model_->SetOutLineColor(color);
	}
}
