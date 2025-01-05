#include "Enemy.h"
#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"

#include "MatrixFunction.h"
#include "VectorFunction.h"
#include <algorithm>
#include <cassert>
#include <numbers>

#include "GlobalVariables/GlobalVariables.h"

#include "CollisionManager.h"
#include "RandomEngine.h"

#include "../Engine/TextureManager.h"

static int startFrame = 0;
static int endFrame = 40;
static int rigidityFrame = 30;
static int attackFrame = 15;


void Enemy::Initialize(const std::vector<HierarchicalAnimation>& models) {
	GlobalVariables* grovalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	grovalVariables->CreateGroup(groupName);
	//grovalVariables->AddItem(groupName, "Test3", Vector3{1.0f,2.0f,3.0f});

	//assert(model);
	BaseCharacter::Initialize(models);
	worldTransform_.translation_.z = 60;
	obb_.center = worldTransform_.translation_;
	obb_.size = {1.0f,1.0f,1.0f};
	//obb_.center.y += obb_.size.y / 2.0f;
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	input_ = Input::GetInstance();
	direction_ = {0,0,-1.0f};
	directionMatrix_ = MakeIdentity4x4();

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
	//worldTransformWepon_.Initialize();
	//worldTransformWepon_.parent_ = &worldTransform_;

	grovalVariables->AddItem(groupName, "Head Translation", models_[kHead].worldTransform_.translation_);
	grovalVariables->AddItem(
	    groupName, "ArmL Translation", models_[kLArm].worldTransform_.translation_);
	grovalVariables->AddItem(
	    groupName, "ArmR Translation", models_[kRArm].worldTransform_.translation_);
	
	grovalVariables->AddItem(groupName, "DashSpeed", dashSpeed_);
	grovalVariables->AddtValue(groupName, "DashLength", dashLength_);

	grovalVariables->AddItem(groupName, "JumpVelocity", kJumpVelocity);
	grovalVariables->AddItem(groupName, "Gravity", kGravity);

	comboNum_ = 0;

	emitter.count = 1;
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;
	emitter.transform.translate = { 0,0,0 };
	bullets_.clear();
	isDead_ = false;
	testAnimation_.reset(new Animation);
	testAnimation_->Initialize();
	testAnimation_->LoadAnimationFile("Resources/Player", "player.gltf");
	testAnimation_->SetPlaySpeed(1.0f);
	testSkeleton_.reset(new Skeleton);
	testSkeleton_->Initialize(models_[kHead].model_->GetRootNode());
	cluster_ = LoadModel::CreateSkinCluster(testSkeleton_->GetSkeletonData(), models_[kHead].model_->GetModelData());

	material_.reset(new Material);
	material_->Initialize();
	material_->paramater_.enableLighting = 2;
	material_->paramater_.shadingType = 1;
	material_->paramater_.disolveThreshold = 0.0f;
	material_->paramater_.environmentCoefficient = 1.0f;
	material_->paramater_.shininess = 40.0f;
	//material_->paramater_.disolveColor = Vector4{ 1.0f, 1.0f, 1.0f, 0.0f };
	material_->paramater_.disolveColor = Vector4{ 5.2f, 0.2f, 0.2f, 0.0f };
	material_->outline_.color = {1.0f,0.0f,0.0f,1.0f};
	material_->ApplyParamater();
}

void Enemy::ReStart() {
	target_ = nullptr;
	behaviorRequest_ = Behavior::kRoot;
	worldTransform_.parent_ = nullptr;
	worldTransform_.rotation_ = {0.0f,0.0f,0.0f};
	worldTransform_.translation_ = {0.0f,0.0f,40.0f};
	//worldTransform_.Initialize();
	direction_ = { 0,0,-1.0f };
	directionMatrix_ = MakeIdentity4x4();
	BehaviorRootInitialize();
	//worldTransform_.UpdateMatrix();
	worldTransform_.matWorld_ = MakeScaleMatrix(worldTransform_.scale_) * directionMatrix_ * MakeTranslateMatrix(worldTransform_.translation_);
	if (worldTransform_.parent_) {
		worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;
	}
	models_[kBody].worldTransform_.rotation_ = { 0,0,0 };
	for (HierarchicalAnimation& model : models_) {
		model.worldTransform_.UpdateMatrix();
	}
	comboNum_ = 0;
	weaponCollider_.SetIsCollision(false);
	bullets_.clear();
	isDead_ = false;
	material_->paramater_.disolveThreshold = 0;
	material_->ApplyParamater();
}

void Enemy::BehaviorRootInitialize() {
	InitializeFloatingGimmick();
	models_[kLArm].worldTransform_.parent_ = &models_[kBody].worldTransform_;
	models_[kRArm].worldTransform_.parent_ = &models_[kBody].worldTransform_;
	models_[kLArm].worldTransform_.translation_.x = -0.5f;
	models_[kLArm].worldTransform_.translation_.y = 1.5f;
	models_[kLArm].worldTransform_.translation_.z = 0.0f;
	models_[kRArm].worldTransform_.translation_.x = 0.5f;
	models_[kRArm].worldTransform_.translation_.y = 1.5f;
	models_[kRArm].worldTransform_.translation_.z = 0.0f;

	models_[kLArm].worldTransform_.rotation_.x = 0.0f;
	models_[kLArm].worldTransform_.rotation_.y = 0.0f;
	models_[kRArm].worldTransform_.rotation_.x = 0.0f;
	models_[kRArm].worldTransform_.rotation_.y = 0.0f;
	comboNum_ = 0;
}

void Enemy::BehaviorAttackInitialize() {

	models_[kLArm].worldTransform_.translation_.x = 0.009f;
	models_[kLArm].worldTransform_.translation_.y = 2.528f;
	models_[kLArm].worldTransform_.translation_.z = -1.20f;
	models_[kRArm].worldTransform_.translation_.x = -0.009f;
	models_[kRArm].worldTransform_.translation_.y = 2.00f;
	models_[kRArm].worldTransform_.translation_.z = -1.20f;
	
	models_[kLArm].worldTransform_.rotation_.x = -1.57f;
	models_[kLArm].worldTransform_.rotation_.y = 0.600f;
	models_[kRArm].worldTransform_.rotation_.x = -1.57f;
	models_[kRArm].worldTransform_.rotation_.y = -0.600f;

	//models_[kRArm].worldTransform_.rotation_.x = -float(std::numbers::pi);
	
	attackBehavior_ = AttackBehavior::kPre;
	weaponCollider_.SetIsCollision(true);
	velocity_ = { 0,0,0 };
	isCoenectCombo_ = false;
}

void Enemy::BehaviorDashInitialize() {
	velocity_ = { 0,0,0 };
}

void Enemy::BehaviorJumpInitialize() {
	velocity_ = {0,0,0};
	acceleration_ = {0,0,0};
}

void Enemy::Update() {
	ApplyGlobalVariables();
	if (!isDead_) {
		//弾削除
		bullets_.remove_if([](std::unique_ptr<Bullet>& bullet) {
			if (bullet->GetIsDead()) {
				return true;
			}
			return false;
			});
		
		//ビヘイビアセット
		if (behaviorRequest_) {
			behavior_ = behaviorRequest_.value();
			frameCount_ = 0;
			switch (behavior_) {
			case Enemy::Behavior::kRoot:
			default:
				BehaviorRootInitialize();
				break;
			case Enemy::Behavior::kAttack:
				BehaviorAttackInitialize();
				break;
			case Enemy::Behavior::kDash:
				BehaviorDashInitialize();
				break;
			case Enemy::Behavior::kJump:
				BehaviorJumpInitialize();
				break;
			}
			behaviorRequest_ = std::nullopt;
		}
		
		//親があったら落下を止める
		if (worldTransform_.parent_) {
			if (worldTransform_.translation_.y < 0.0f) {
				worldTransform_.translation_.y = 0.0f;
			}

		}
	
		//ビヘイビア実行
		switch (behavior_) {
		case Enemy::Behavior::kRoot:
			BehaviorRootUpdate();
			break;
		case Enemy::Behavior::kAttack:
			BehaviorAttackUpdate();
			break;
		case Enemy::Behavior::kDash:
			BehaviorDashUpdate();
			break;
		case Enemy::Behavior::kJump:
			BehaviorJumpUpdate();
			break;
		}
	}
	else {
		//死亡アニメーション
		const float kDisolveStep = 0.005f;
		const float kRotateStep = 0.05f;
		const float kRotateLimit = -3.14f / 2.0f;

		if (material_->paramater_.disolveThreshold < 1.0f) {
			material_->paramater_.disolveThreshold += kDisolveStep;
		}

		if (models_[kBody].worldTransform_.rotation_.x > kRotateLimit) {
			models_[kBody].worldTransform_.rotation_.x -= kRotateStep;
		}
	}
	// 行列更新
	worldTransform_.matWorld_ = MakeScaleMatrix(worldTransform_.scale_) * directionMatrix_ * MakeTranslateMatrix(worldTransform_.translation_);
	if (worldTransform_.parent_){
		worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;
	}
	// キャラクタの座標を表示する
	float* slider3[3] = {
	    &worldTransform_.translation_.x, &worldTransform_.translation_.y,
	    &worldTransform_.translation_.z};

	//当たり判定更新
	obb_.center = worldTransform_.GetWorldPosition();
	obb_.size = { 0.5f,1.0f,0.5f };
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	
	//更新処理
	for (HierarchicalAnimation& model : models_) {
		model.worldTransform_.UpdateMatrix();
	}

	for (std::list<std::unique_ptr<Bullet>>::iterator iterator = bullets_.begin();
		iterator != bullets_.end(); iterator++) {
		(*iterator)->Update();
	}


	material_->ApplyParamater();
	preJoyState_ = joyState_;
}

void Enemy::BehaviorRootUpdate()
{
	//ステート変更
	if (data_.behavior == Behavior::kAttack)
	{
		behaviorRequest_ = Behavior::kAttack;
	}
	if (data_.behavior == Behavior::kDash)
	{
		behaviorRequest_ = Behavior::kDash;
	}
	if ((data_.behavior == Behavior::kJump) &&
		worldTransform_.parent_)
	{
		behaviorRequest_ = Behavior::kJump;
	}
	
	const float kCharacterSpeed = 0.1f;

	//移動処理
	Vector3 move = data_.move;
	move = Normalize(move) * kCharacterSpeed;
	Matrix4x4 newDirection = DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(move));
	directionMatrix_ = newDirection;
	direction_ = move;
	
	//ターゲットの方を向く
	if (target_) {
		Vector3 toTarget = target_->GetWorldPosition() - worldTransform_.GetWorldPosition();
		toTarget.y = 0;
		directionMatrix_=DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(toTarget));
	}
	worldTransform_.translation_ += move;

	//移動時パーティクル発生
	if (worldTransform_.parent_ && particle_ && Length(move) !=0.0f) {
		Particle::ParticleData particleData;
		for (uint32_t count = 0; count < emitter.count; count++) {
			particleData.transform.scale = { 1.0f,1.0f,1.0f };
			particleData.transform.rotate = { 0.0f,0.0f,0.0f };
			particleData.transform.translate = worldTransform_.GetWorldPosition() + Vector3{ RandomEngine::GetRandom(-1.0f, 1.0f), RandomEngine::GetRandom(-1.0f, 1.0f), RandomEngine::GetRandom(-1.0f, 1.0f) };
			particleData.velocity = { RandomEngine::GetRandom(-1.0f,1.0f),RandomEngine::GetRandom(-1.0f,1.0f), RandomEngine::GetRandom(-1.0f,1.0f) };
			particleData.color = { 1.0f,1.0f,1.0f,1.0f };
			particleData.lifeTime = RandomEngine::GetRandom(1.0f, 3.0f);
			particleData.currentTime = 0;
			particle_->MakeNewParticle(particleData);
		}
	}
	//落下処理
	if (!worldTransform_.parent_) {
		velocity_ += kGravity;
	}
	worldTransform_.translation_ += velocity_;

	//アニメーション
	UpdateFloatingGimmick();
}

void Enemy::BehaviorAttackUpdate()
{
	static float weponRotateEnd = 3.14f;

	static float frontLength = 5.0f;
	
	//待機状態に戻る
	if (frameCount_ == endFrame) {
		weaponCollider_.SetIsCollision(false);
		behaviorRequest_ = Behavior::kRoot;
	}
	//射撃する
	if (target_) {
		Vector3 toTarget = target_->GetWorldPosition() - worldTransform_.GetWorldPosition();
		if (frameCount_ == 0) {
			//shot
			std::unique_ptr<Bullet> bullet;
			bullet.reset(new Bullet());
			bullet->Initialize();
			bullet->SetPosition(models_[kHead].worldTransform_.GetWorldPosition());
			bullet->SetModel(modelBullet_);
			Vector3 velocity = Normalize(toTarget) * 1.5f;
			bullet->SetVelocity(velocity);
			bullet->SetParticle(particle_);
			bullet->SetAnimation(bulletAnimation_);
			bullets_.push_back(std::move(bullet));
		}
		toTarget.y = 0;
		directionMatrix_ = DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(toTarget));
	}

	
	frameCount_++;
}

void Enemy::BehaviorDashUpdate() {
	//回避方向をに回避する
	Vector3 move = {0,0,dashSpeed_};
	Matrix4x4 rotate = worldTransform_.matWorld_;
	rotate.m[3][0] = 0;
	rotate.m[3][1] = 0;
	rotate.m[3][2] = 0;
	if (target_) {
		rotate = DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(direction_));
	}
	move = Transform(move, rotate);
	worldTransform_.translation_ += move;

	//待機状態に戻る
	if (frameCount_ >= dashLength_) {
		behaviorRequest_ = Behavior::kRoot;
	}
	frameCount_++;
}

void Enemy::BehaviorJumpUpdate() {
	velocity_ = kJumpVelocity * directionMatrix_;
	behaviorRequest_ = Behavior::kRoot;
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	//アニメーション適用
	if (!isDead_) {
		testAnimation_->Update();
	}
	testSkeleton_->ApplyAnimation(*testAnimation_->GetAnimationData().get(), testAnimation_->GetTime());
	testSkeleton_->Update();
	LoadModel::UpdateSkinCluster(cluster_, testSkeleton_->GetSkeletonData());

	//各要素描画
	for (uint32_t index = 0; index < models_.size(); index++) {

		models_[index].model_->SetMaterial(material_.get());
		if (index == kHead) {
			models_[index].model_->Draw(models_[index].worldTransform_, viewProjection, cluster_);
		}
		else {
			models_[index].model_->Draw(models_[index].worldTransform_, viewProjection);
		}
	}
	//自身の持つ弾を描画
	for (std::list<std::unique_ptr<Bullet>>::iterator iterator = bullets_.begin();
		iterator != bullets_.end(); iterator++) {
		(*iterator)->Draw(viewProjection);
	}
}

void Enemy::DrawOutLine(const ViewProjection& viewProjection)
{
	if (material_->paramater_.disolveThreshold == 0.0f) {
		for (uint32_t index = 0; index < models_.size(); index++) {
			if (index == kHead) {
				models_[index].model_->DrawOutLine(models_[index].worldTransform_, viewProjection, cluster_);
			}
			else {
				models_[index].model_->DrawOutLine(models_[index].worldTransform_, viewProjection);
			}
		}
	}
}

void Enemy::InitializeFloatingGimmick() {

	floatingParameter_ = 0.0f;
}


void Enemy::UpdateFloatingGimmick()
{
	static uint16_t period = 120;
	const float step = 2.0f * float(std::numbers::pi) / float(period);

	floatingParameter_ += step;

	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * float(std::numbers::pi));

	static float floatingAmplitude = 0.5;
	models_[kBody].worldTransform_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude;
	models_[kLArm].worldTransform_.rotation_.x = std::cos(floatingParameter_) * floatingAmplitude;
	models_[kRArm].worldTransform_.rotation_.x = std::cos(floatingParameter_) * floatingAmplitude;

}

void Enemy::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	models_[kHead].worldTransform_.translation_ = globalVariables->GetVector3Value(groupName, "Head Translation");
	models_[kLArm].worldTransform_.translation_ =
	    globalVariables->GetVector3Value(groupName, "ArmL Translation");
	models_[kRArm].worldTransform_.translation_ =
	    globalVariables->GetVector3Value(groupName, "ArmR Translation");
	dashSpeed_ = globalVariables->GetFloatValue(groupName,"DashSpeed");
	dashLength_ = globalVariables->GetIntValue(groupName, "DashLength");
	kJumpVelocity = globalVariables->GetVector3Value(groupName, "JumpVelocity");
	kGravity = globalVariables->GetVector3Value(groupName, "Gravity");
}

void Enemy::OnCollision(WorldTransform& parent)
{
	if (worldTransform_.parent_ != &parent) {
		Matrix4x4 rocal = worldTransform_.matWorld_ * (Inverse(parent.matWorld_));
		worldTransform_.translation_.x = rocal.m[3][0];
		worldTransform_.translation_.y = rocal.m[3][1];
		worldTransform_.translation_.z = rocal.m[3][2];

		worldTransform_.parent_ = &parent;
		velocity_ = { 0,0,0 };
		isFlooar_ = true;
	}
}

void Enemy::OutCollision() {
	if (worldTransform_.parent_) {
		worldTransform_.parent_ = nullptr;
		Matrix4x4 world = worldTransform_.matWorld_;
		worldTransform_.translation_.x = world.m[3][0];
		worldTransform_.translation_.y = world.m[3][1];
		worldTransform_.translation_.z = world.m[3][2];
	}
}