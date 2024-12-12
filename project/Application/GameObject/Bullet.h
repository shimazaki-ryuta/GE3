#pragma once
#include "3D/WorldTransform.h"
#include "3D/ViewProjection.h"
#include "3D/Model.h"
#include "Colider/Sphere.h"
#include "3D/Particle.h"
#include "3D/Animation.h"

//プレイヤー(+敵)の発射する弾のクラス

class Bullet
{
public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(const ViewProjection& viewProjection);

	//Getter/Setter
	void SetVelocity(const Vector3& vec) { velocity_ = vec; };
	bool GetIsDead() { return isDead_; };
	void SetIsDead(bool is) { isDead_ = is; };
	Sphere& GetSphere() { return sphere_; };
	void SetModel(Model* model) { model_ = model; };
	void SetPosition(const Vector3& pos) { worldTransform_.translation_ = pos; };
	void SetParticle(Particle* particle) { particle_ = particle; };
	void SetAnimation(Animation* animation) { animation_->SetAnimationData(animation->GetAnimationData()); };
private:
	WorldTransform worldTransform_;
	Model* model_;
	Sphere sphere_;
	Vector3 velocity_;
	bool isDead_;
	int aliveTime_;
	Particle* particle_;
	std::unique_ptr<Animation> animation_;
};

