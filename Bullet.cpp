#include "Bullet.h"
#include "Math/VectorFunction.h"
void Bullet::Initialize() {
	worldTransform_.Initialize();
	isDead_ = false;
	aliveTime_ = 180;
}

void Bullet::Update() {
	worldTransform_.translation_ += velocity_;
	worldTransform_.UpdateMatrix();
	sphere_.center = worldTransform_.GetWorldPosition();
	sphere_.radius = worldTransform_.scale_.x;
	if (aliveTime_<=0) {
		isDead_ = true;
	}
	aliveTime_--;
}

void Bullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_,viewProjection);
}