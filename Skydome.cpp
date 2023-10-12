#include "Skydome.h"

void Skydome::Initialize(Model* model, const Vector3& position) {
	model_.reset(model);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Skydome::Update()
{

}

void Skydome::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}