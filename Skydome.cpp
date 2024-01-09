#include "Skydome.h"
#include "TextureManager.h"
void Skydome::Initialize(Model* model, const Vector3& position) {
	model_.reset(model);
	model->SetEnableLighting(0);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {10.0f,10.0f,10.0f};
	worldTransform_.UpdateMatrix();
	textureHandle_ = TextureManager::LoadTexture("sky2.png");
}

void Skydome::Update()
{

}

void Skydome::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection,textureHandle_);
}