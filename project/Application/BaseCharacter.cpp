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
	for (HierarchicalAnimation& model : models_) {
		model.model_->Draw(worldTransform_, viewProjection);
	}
}
