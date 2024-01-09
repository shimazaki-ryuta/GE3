#include "Bullet.h"
#include "Math/VectorFunction.h"
#include "RandomEngine.h"
void Bullet::Initialize() {
	worldTransform_.Initialize();
	//worldTransform_.scale_ = {0.8f,0.8f,0.8f};
	isDead_ = false;
	aliveTime_ = 180;
}

void Bullet::Update() {
	worldTransform_.translation_ += velocity_;
	worldTransform_.UpdateMatrix();
	sphere_.center = worldTransform_.GetWorldPosition();
	sphere_.radius = worldTransform_.scale_.x;

	Particle::ParticleData particleData;
		particleData.transform.scale = { 0.5f,0.5f,0.5f };
		particleData.transform.rotate = { 0.0f,0.0f,0.0f };
		particleData.transform.translate = worldTransform_.GetWorldPosition();
		particleData.velocity = {0,0,0};
		particleData.color = { RandomEngine::GetRandom(0.0f,1.0f),0,0,1.0f };
		particleData.lifeTime = RandomEngine::GetRandom(1.0f, 3.0f);
		particleData.currentTime = 0;
		particle_->MakeNewParticle(particleData);

	if (aliveTime_<=0) {
		isDead_ = true;
	}
	aliveTime_--;
}

void Bullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_,viewProjection);
}