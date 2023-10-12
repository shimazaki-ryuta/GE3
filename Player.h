#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>
#include "BaseCharacter.h"
#include <optional>

class Player : public BaseCharacter {
public:
	enum class Behavior { 
		kRoot,
		kAttack,
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

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	void BehaviorRootUpdate();
	void BehaviorAttackUpdate();

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
	inline void SetWepon(Model* model) { modelWepon_ = model; };
	
	void ApplyGlobalVariables();

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

	//調整用
	int frameCount_ = 0;
};
