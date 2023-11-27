#pragma once
#include "BaseCharacter.h"
#include "Sphere.h"
#include "collider.h"
#include "FollowCamera.h"
class Enemy : public BaseCharacter
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::vector<HierarchicalAnimation>& models) override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(const ViewProjection& ) override;

	Sphere& GetSphere() { return sphere_; };

	void OnCollision();
	void ReStart() {
		isDead_ = false;
		hitPoint_ = 3;
		worldTransform_.scale_ = {1.0f,1.0f,1.0f};
		worldTransform_.rotation_ = {0,0,0};
		position_ = {0,0,0};
	};
	bool IsDead() { return isDead_; };
	void SetOffset(const Vector3& offset) { offset_ = offset; };
	void setTarget(WorldTransform* target) { target_ = target; };
	int GetHP() { return hitPoint_; };
	void SetCamera(FollowCamera* camera) { camera_ = camera; };
private:
	Vector3 position_;

	Sphere sphere_;

	bool isDead_;
	Collider sphereCollider_;
	Vector3 offset_;
	int hitPoint_;

	WorldTransform* target_;
	Vector3 veloity_;
	Vector3 gravity_;
	int invincibleTime_;
	FollowCamera* camera_;
};
