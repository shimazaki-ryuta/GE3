#include "Enemy.h"
//#include "ImGuiManager.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include "CollisionManager.h"
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
	position_ = {0.0f,0.0f,0.0f};
	sphere_.center = worldTransform_.translation_;
	sphere_.radius = 0.5f;

	isDead_ = false;
	sphereCollider_.SetSphere(sphere_);
	sphereCollider_.SetOnCollisionEvent(std::bind(&Enemy::OnCollision,this));
	CollisionManager::GetInstance()->PushCollider(&sphereCollider_);
}

void Enemy::Update() { 
	if (!isDead_) {
		const float rotateY = 0.05f;
		worldTransform_.rotation_.y += rotateY;
		Vector3 move = { 0.0f,0.0f,0.2f };
		//Vector3 offset = { -4.0f,0.0f,44.0f };

		position_ += Transform(move, MakeRotateMatrix(worldTransform_.rotation_));
		worldTransform_.translation_ = position_ + offset_;
		sphere_.center = worldTransform_.translation_;
		sphere_.radius = 0.5f;
		sphereCollider_.SetSphere(sphere_);
		models_[1].worldTransform_.rotation_.x += 0.1f;
		BaseCharacter::Update();
		for (HierarchicalAnimation& model : models_) {
			model.worldTransform_.UpdateMatrix();
		}
	}
	
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	// model_->Draw(worldTransform_, viewProjection);
	//BaseCharacter::Draw(viewProjection);
	if (!isDead_) {
		for (HierarchicalAnimation& model : models_) {
			model.model_->Draw(model.worldTransform_, viewProjection);
		}
	}
}

