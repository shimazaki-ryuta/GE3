#include "Ground.h"

void Ground::Initialize(Model* model, const Vector3& position) {
	model_.reset(model);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	model_->SetEnableLighting(2);
	model_->SetShiniess(40.0f);
	testAnimation_.reset(new Animation);
	testAnimation_->Initialize();
	testAnimation_->LoadAnimationFile("Resources/bullet", "bullet.gltf");
	testAnimation_->SetPlaySpeed(5.0f);
}

void Ground::Update() {
}

void Ground::Draw(const ViewProjection& viewProjection) {
	testAnimation_->Update();
	model_->SetLocalMatrix(testAnimation_->GetAnimationMatrix("center"));
	model_->Draw(worldTransform_, viewProjection);
}
void Ground::DrawOutLine(const ViewProjection& viewProjection) {
	model_->DrawOutLine(worldTransform_, viewProjection);
}