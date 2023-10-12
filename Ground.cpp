#include "Ground.h"

void Ground::Initialize(Model* model, const Vector3& position) {
	model_.reset(model);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Ground::Update() {}

void Ground::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}