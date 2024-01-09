#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>
#include "BaseCharacter.h"
#include <optional>
#include "OBB.h"
#include "Collider.h"
#include "input.h"
#include "Particle.h"
#include "Bullet.h"
class Player : public BaseCharacter {
public:
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
	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();



/*
	inline WorldTransform* GetWorldTransform() { return &worldTransform_; };
	*/
	inline void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	};
	//inline void SetWepon(Model* model) { modelWepon_ = model; };
	
	void ApplyGlobalVariables();

	void OnCollision(WorldTransform& parent);
	void OutCollision();

	void OnCollisionEnemy() { ReStart(); };

	OBB& GetOBB() { return obb_; };

	void ReStart();
	void SetTarget(WorldTransform* target) { target_ = target; };
	void SetParticle(Particle* particle) { particle_ = particle; };
	void SetModelBullet(Model* model) { modelBullet_ = model; };

	void SetFire(std::function<void()> func) { fire_ = func; };

	std::list<std::unique_ptr<Bullet>>& GetBulletList() { return bullets_; };
	void SetIsDead(bool is) { isDead_ = is; };
	bool GetIsDead() { return isDead_; };
private:
	//WorldTransform worldTransform_;
	const ViewProjection* viewProjection_ = nullptr;
	//Model* model_ = nullptr;
	//Model* modelBody_  = nullptr;
	//Model* modelHead_  = nullptr;
	//Model* modelL_arm_ = nullptr;
	//Model* modelR_arm_ = nullptr;
	//std::vector<Model*> models_;
	//std::vector <HierarchicalAnimation> models_;
	WorldTransform worldTransformWepon_;
	Model* modelWepon_;

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

	//OBB weaponOBB_;
	//Collider weaponCollider_;

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

	//std::unique_ptr<Model> obbModel_;
	//bool isDrawOBB_=false;
	//WorldTransform worldTtansformOBB_;

	XINPUT_STATE joyState_;
	//1フレーム前の入力情報
	XINPUT_STATE preJoyState_;

	//コンボ
	uint32_t comboNum_;
	bool isCoenectCombo_;

	WorldTransform* target_;
	Particle* particle_;
	Particle::Emitter emitter;

	std::list<std::unique_ptr<Bullet>> bullets_;
	Model* modelBullet_;

	std::function<void()> fire_;
	bool isDead_;
};
