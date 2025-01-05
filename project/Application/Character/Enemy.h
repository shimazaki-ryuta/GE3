#pragma once
#include "Input.h"
#include "3D/Model.h"
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"
#include <vector>
#include "BaseCharacter.h"
#include <optional>
#include "OBB.h"
#include "Collider.h"
#include "input.h"
#include "3D/Particle.h"
#include "GameObject/Bullet.h"
#include "3D/Animation.h"
#include "3D/Skeleton.h"
#include "3D/Material.h"

//敵の制御、描画を行うクラス

class Enemy : public BaseCharacter {
public:

	enum PlayerPart {
		kBody,
		kHead,
		kLArm,
		kRArm,
	};

	enum class Behavior { 
		kRoot,
		kAttack,
		kDash,
		kJump,
	};

	enum class AttackBehavior {
		kPre,
		kAttack,
		kEnd,
	};

	struct ReceveData
	{
		Vector3 move;
		Behavior behavior;
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::vector<HierarchicalAnimation>& models) override;
	void BehaviorRootInitialize();
	void BehaviorAttackInitialize();
	void BehaviorDashInitialize();
	void BehaviorJumpInitialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	void BehaviorRootUpdate();
	void BehaviorAttackUpdate();
	void BehaviorDashUpdate();
	void BehaviorJumpUpdate();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(const ViewProjection& viewProjection) override;
	void DrawOutLine(const ViewProjection& viewProjection);
	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();

	//グローバル変数適用
	void ApplyGlobalVariables();

	//衝突時
	void OnCollision(WorldTransform& parent);
	void OutCollision();
	void OnCollisionEnemy() { ReStart(); };

	//リセット処理
	void ReStart();

	//Getter/Setter
	inline void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; };
	OBB& GetOBB() { return obb_; };
	void SetTarget(WorldTransform* target) { target_ = target; };
	void SetParticle(Particle* particle) { particle_ = particle; };
	void SetData(const ReceveData& d) {data_ = d;};
	WorldTransform* GetWorldTransformBody() { return &models_[1].worldTransform_; };
	Behavior GetBehavior() {return behavior_;};
	void SetModelBullet(Model* model) { modelBullet_ = model; };
	std::list<std::unique_ptr<Bullet>>& GetBulletList() { return bullets_; };
	void SetIsDead(bool is) { isDead_ = is; };
	bool GetIsDead() { return isDead_; };
	void SetBulletAnimation(Animation* animation) { bulletAnimation_ = animation; };
private:
	const ViewProjection* viewProjection_ = nullptr;

	uint32_t textureHandle_ = 0u;

	float floatingParameter_ = 0.0f;

	Input* input_ = nullptr;

	//uint16_t period = 120;
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	AttackBehavior attackBehavior_ = AttackBehavior::kPre;

	bool isFlooar_ = false;

	OBB obb_;

	//調整用
	int frameCount_ = 0;

	//向き
	Vector3 direction_;
	Matrix4x4 directionMatrix_;

	OBB weaponOBB_;
	Collider weaponCollider_;

	//ダッシュ用変数
	//ダッシュ中の速度
	float dashSpeed_;
	//ダッシュのフレーム数
	int dashLength_;

	//ジャンプ用変数
	Vector3 velocity_;
	Vector3 acceleration_;
	Vector3 kGravity;
	Vector3 kJumpVelocity;

	XINPUT_STATE joyState_;
	//1フレーム前の入力情報
	XINPUT_STATE preJoyState_;

	//コンボ
	uint32_t comboNum_;
	bool isCoenectCombo_;

	WorldTransform* target_;
	Particle* particle_;
	Particle::Emitter emitter;

	ReceveData data_;

	std::list<std::unique_ptr<Bullet>> bullets_;
	Model* modelBullet_;
	bool isDead_;
	Animation* bulletAnimation_;

	std::unique_ptr<Animation> testAnimation_;
	std::unique_ptr<Skeleton> testSkeleton_;
	SkinCluster cluster_;
	std::unique_ptr<Material> material_;
};
