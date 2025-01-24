#include "Player.h"
//#include "ImGuiManager.h"
//ImGui
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
#include "Collision.h"

static int rigidityFrame = 30;
static int attackFrame = 15;


void Player::Initialize(const std::vector<HierarchicalAnimation>& models) {
	GlobalVariables* grovalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	grovalVariables->CreateGroup(groupName);

	BaseCharacter::Initialize(models);
	worldTransform_.translation_.z = -60.0f;
	obb_.center = worldTransform_.translation_;
	obb_.size = {1.0f,1.0f,1.0f};
	obb_.center.y += obb_.size.y / 2.0f;
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	input_ = Input::GetInstance();
	direction_ = {0,0,1.0f};
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
	worldTransformWepon_.Initialize();
	worldTransformWepon_.parent_ = &worldTransform_;

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
	testAnimation_->SetPlaySpeed(3.0f);
	testSkeleton_.reset(new Skeleton);
	testSkeleton_->Initialize(models_[kHead].model_->GetRootNode());
	cluster_ = LoadModel::CreateSkinCluster(testSkeleton_->GetSkeletonData(), models_[kHead].model_->GetModelData());

	material_.reset(new Material);
	material_->Initialize();
	material_->paramater_.enableLighting = 2;
	material_->paramater_.shadingType = 1;
	material_->paramater_.disolveThreshold = 0.0f;
	material_->paramater_.environmentCoefficient = 0.0f;
	//material_->paramater_.disolveColor = Vector4{ 1.0f, 1.0f, 1.0f, 0.0f };
	material_->paramater_.disolveColor = Vector4{ 0.2f, 0.2f, 5.2f, 0.0f };
	material_->outline_.color = { 0.0f,0.0f,1.0f,1.0f };
	material_->ApplyParamater();


	//ステート
	stateFactory_ = std::make_unique<PlayerStateFactory>();
	ChangeState("ROOT");
}

void Player::ReStart() {
	target_ = nullptr;
	behaviorRequest_ = Behavior::kRoot;
	worldTransform_.parent_ = nullptr;
	worldTransform_.rotation_ = {0.0f,0.0f,0.0f};
	worldTransform_.translation_ = {0.0f,0.0f,-40.0f};
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
	direction_ = { 0,0,1.0f };
	directionMatrix_ = MakeIdentity4x4();
	comboNum_ = 0;
	//weaponCollider_.SetIsCollision(false);
	bullets_.clear();
	isDead_ = false;
	material_->paramater_.disolveThreshold = 0;
	material_->ApplyParamater();
}

void Player::BehaviorRootInitialize() {
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

void Player::BehaviorAttackInitialize() {

	worldTransformWepon_.translation_.y = 1.0f;
	worldTransformWepon_.rotation_.x = 0.0f;
	worldTransformWepon_.rotation_.z = -1.57f;

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
	//weaponCollider_.SetIsCollision(true);
	velocity_ = { 0,0,0 };
	isCoenectCombo_ = false;
}

void Player::BehaviorDashInitialize() {
	velocity_ = { 0,0,0 };
}

void Player::BehaviorJumpInitialize() {
	velocity_ = {0,0,0};
	acceleration_ = {0,0,0};
}

void Player::Update() {
	Input::GetInstance()->GetJoystickState(0, joyState_);
	ApplyGlobalVariables();
	//弾削除
	bullets_.remove_if([](std::unique_ptr<Bullet>& bullet) {
		if (bullet->GetIsDead()) {
			return true;
		}
		return false;
		});
	if (!isDead_) {		

		//ステート更新
		if (state_) {
			state_->Update();
		}

		//親があったら落下を止める
		if (worldTransform_.parent_) {
			if (worldTransform_.translation_.y < 0.0f) {
				worldTransform_.translation_.y = 0.0f;
			}
		}

	}
	else {

		//死亡時アニメーション

		const float kDisolveStep = 0.005f;
		const float kRotateStep = 0.05f;
		const float kRotateLimit = -3.14f / 2.0f;

		if (material_->paramater_.disolveThreshold < 1.0f) {
			material_->paramater_.disolveThreshold += kDisolveStep;
		}

		if (models_[0].worldTransform_.rotation_.x > kRotateLimit) {
			models_[0].worldTransform_.rotation_.x -= kRotateStep;
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
	obb_.size = { 1.0f,1.0f,1.0f };
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

void Player::ToTarget() {
	//敵の方を向く
	if (target_) {
		Vector3 toTarget = target_->GetWorldPosition() - worldTransform_.GetWorldPosition();
		toTarget.y = 0;
		directionMatrix_ = DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(toTarget));
	}
}


void Player::Shot(){
	const float kVelocityCefficent = 1.5f;
	if (target_) {
		Vector3 toTarget = target_->GetWorldPosition() - models_[kHead].worldTransform_.GetWorldPosition();
		if (frameCount_ == 0) {
			//shot
			std::unique_ptr<Bullet> bullet;
			bullet.reset(new Bullet());
			bullet->Initialize();
			bullet->SetPosition(models_[kHead].worldTransform_.GetWorldPosition());
			bullet->SetModel(modelBullet_);
			Vector3 velocity = Normalize(toTarget) * kVelocityCefficent;
			bullet->SetVelocity(velocity);
			bullet->SetParticle(particle_);
			bullet->SetAnimation(bulletAnimation_);
			bullets_.push_back(std::move(bullet));
			fire_();
		}
		toTarget.y = 0;
		directionMatrix_ = DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(toTarget));
	}
}


void Player::Draw(const ViewProjection& viewProjection) {
	//アニメーション適用
	if (!isDead_) {
		testAnimation_->Update();
	}
	testSkeleton_->ApplyAnimation(*testAnimation_->GetAnimationData().get(), testAnimation_->GetTime());
	testSkeleton_->Update();
	LoadModel::UpdateSkinCluster(cluster_, testSkeleton_->GetSkeletonData());
	
	//各要素描画
	for (uint32_t index = 0; index < models_.size();index++) {

		models_[index].model_->SetMaterial(material_.get());
		if (index == kHead) {
			models_[index].model_->Draw(models_[index].worldTransform_, viewProjection,cluster_);
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

void Player::DrawOutLine(const ViewProjection& viewProjection)
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

void Player::InitializeFloatingGimmick() {

	floatingParameter_ = 0.0f;
}


void Player::UpdateFloatingGimmick()
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

void Player::ApplyGlobalVariables()
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

void Player::OnCollision(WorldTransform& parent)
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

void Player::OutCollision() {
	if (worldTransform_.parent_) {
		worldTransform_.parent_ = nullptr;
		Matrix4x4 world = worldTransform_.matWorld_;
		worldTransform_.translation_.x = world.m[3][0];
		worldTransform_.translation_.y = world.m[3][1];
		worldTransform_.translation_.z = world.m[3][2];
		isFlooar_ = false;
	}
}

void Player::OnCollisionSphere(WorldTransform& parent, Sphere partner) {
	PushBack(0,obb_,partner);
	Matrix4x4 rocal = MakeTranslateMatrix(obb_.center);
	if (worldTransform_.parent_) {
		rocal *= (Inverse(parent.matWorld_));
	}
	worldTransform_.translation_.x = rocal.m[3][0];
	worldTransform_.translation_.y = rocal.m[3][1];
	worldTransform_.translation_.z = rocal.m[3][2];

	worldTransform_.parent_ = &parent;
	worldTransform_.UpdateMatrix();
	velocity_ = { 0,0,0 };
	isFlooar_ = true;
}

bool Player::GetIsJump() {
	bool isJump = false;
	if (worldTransform_.parent_) {
		isJump = true;
	}
	return isJump;
};

void Player::ChangeState(const std::string& stateName) {
	state_.reset(stateFactory_->CreateState(stateName));
	state_->SetPlayer(this);
	state_->Initialize();
}