#include "PlayerBullet.h"
#include "VectorFunction.h"
#include <assert.h>
#include "Enemy.h"

void PlayerBullet::Initialize(
    Model* model, const Vector3& position, const Vector3& velocity,std::weak_ptr<Enemy> enemy) {
	assert(model);
	model_ = model;
	textureHandle_ = TextureManager::Load("black.png");
	velocity_ = velocity;
	worldTransForm_.Initialize();
	//引数で受け取った初期座標をセット
	worldTransForm_.translation_ = position;
	enemy_ = enemy;
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);

	worldTransForm_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{velocity_.x, 0.0f, velocity_.z};
	float besage = Length(velocityXZ);
	worldTransForm_.rotation_.x = std::atan2(-velocity_.y, besage);
}

void PlayerBullet::Update()
{
	if (std::shared_ptr<Enemy> enemy = enemy_.lock()) 
	{
		Vector3 toEnemy = enemy->GetWorldPosition() - worldTransForm_.translation_;

		float speed = 0.2f;

		velocity_ = Slerp(velocity_, toEnemy, 0.05f) * speed;

	}

	worldTransForm_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{velocity_.x, 0.0f, velocity_.z};
	float besage = Length(velocityXZ);
	worldTransForm_.rotation_.x = std::atan2(-velocity_.y, besage);

	worldTransForm_.translation_ += velocity_;

	//行列を更新
	worldTransForm_.UpdateMatrix(); 

	//時間経過でデス
	if (--deathTimer_ <= 0)
	{
		isDead_ = true;
	}

}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransForm_, viewProjection, textureHandle_);
}

Vector3 PlayerBullet::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransForm_.matWorld_.m[3][0];
	worldPos.y = worldTransForm_.matWorld_.m[3][1];
	worldPos.z = worldTransForm_.matWorld_.m[3][2];
	return worldPos;
}

void PlayerBullet::OnCollision() 
{
	isDead_ = true;
}