#include "Enemy.h"
//#include "ImGuiManager.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include <algorithm>
#include <cassert>
#include <numbers>
void Enemy::Initialize(const std::vector<HierarchicalAnimation>& models) {
	// assert(model);
	BaseCharacter::Initialize(models);
	for (HierarchicalAnimation& model_ : models_) {
		model_.worldTransform_.Initialize();
		model_.worldTransform_.UpdateMatrix();
	}
	std::vector<HierarchicalAnimation>::iterator body = models_.begin();
	body->worldTransform_.parent_ = &worldTransform_;
	for (std::vector<HierarchicalAnimation>::iterator childlen = models_.begin() + 1;
	     childlen != models_.end(); childlen++) {
		childlen->worldTransform_.parent_ = &(body->worldTransform_);
	}
}

void Enemy::Update() { 

	const float rotateY = 0.05f;
	worldTransform_.rotation_.y += rotateY;
	Vector3 move = {0.0f,0.0f,0.5f};
	worldTransform_.translation_ += Transform(move, MakeRotateMatrix(worldTransform_.rotation_));
	models_[1].worldTransform_.rotation_.x += 0.1f;
	BaseCharacter::Update();
	for (HierarchicalAnimation& model : models_) {
		model.worldTransform_.UpdateMatrix();
	}
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	// model_->Draw(worldTransform_, viewProjection);
	//BaseCharacter::Draw(viewProjection);
	for (HierarchicalAnimation& model : models_) {
		model.model_->Draw(model.worldTransform_, viewProjection);
	}
	
}

