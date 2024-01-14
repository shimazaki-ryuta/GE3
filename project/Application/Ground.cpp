#include "Ground.h"

void Ground::Initialize(Model* model, const Vector3& position) {
	model_.reset(model);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	model_->SetEnableLighting(2);
	model_->SetShiniess(40.0f);
}

void Ground::Update() {}

void Ground::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}