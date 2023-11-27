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

	hitPoint_ = 3;
}

void Enemy::Update() {
	if (!isDead_) {
		if (hitPoint_>0) {
			const float rotateY = 0.05f;
			worldTransform_.rotation_.y += rotateY;
			Vector3 move = { 0.0f,0.0f,0.2f };
			//Vector3 offset = { -4.0f,0.0f,44.0f };

			position_ += Transform(move, MakeRotateMatrix(worldTransform_.rotation_));
			worldTransform_.translation_ = position_ + offset_;
			//worldTransform_.translation_.y += 0.5f;
			sphere_.center = worldTransform_.translation_;
			sphere_.radius = 0.5f;
			sphereCollider_.SetSphere(sphere_);
			models_[1].worldTransform_.rotation_.x += 0.1f;
			invincibleTime_--;
		}
		else {
			veloity_ += gravity_;
			worldTransform_.translation_ += veloity_;
			worldTransform_.scale_ *= 0.95f;
			if (worldTransform_.scale_.x < 0.1f) {
				isDead_ = true;
			}
		}
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

void Enemy::OnCollision() {
	//isDead_ = true;
	if (invincibleTime_<=0) {
		hitPoint_--;
		if (camera_) {
			camera_->Shake();
		}
		invincibleTime_ = 40;
	}
	if (hitPoint_ == 0 ) {
		if (target_) {
			Vector3 direction = worldTransform_.GetWorldPosition() - target_->GetWorldPosition();
			direction = Normalize(direction);
			veloity_ = direction * 1.0f;
			veloity_.y = 0.4f;
			gravity_ = {0,-0.05f,0};
		}
	}
}
