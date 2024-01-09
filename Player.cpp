#include "Player.h"
//#include "ImGuiManager.h"
//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include "MatrixFunction.h"
#include "VectorFunction.h"
#include <algorithm>
#include <cassert>
#include <numbers>

#include "GlobalVariables.h"

#include "CollisionManager.h"
#include "RandomEngine.h"

static int startFrame = 0;
static int endFrame = 40;
static int rigidityFrame = 30;
static int attackFrame = 15;


void Player::Initialize(const std::vector<HierarchicalAnimation>& models) {
	GlobalVariables* grovalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	grovalVariables->CreateGroup(groupName);
	//grovalVariables->AddItem(groupName, "Test3", Vector3{1.0f,2.0f,3.0f});


	//assert(model);
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

	grovalVariables->AddItem(groupName, "Head Translation", models_[1].worldTransform_.translation_);
	grovalVariables->AddItem(
	    groupName, "ArmL Translation", models_[2].worldTransform_.translation_);
	grovalVariables->AddItem(
	    groupName, "ArmR Translation", models_[3].worldTransform_.translation_);
	
	grovalVariables->AddItem(groupName, "DashSpeed", dashSpeed_);
	grovalVariables->AddtValue(groupName, "DashLength", dashLength_);

	grovalVariables->AddItem(groupName, "JumpVelocity", kJumpVelocity);
	grovalVariables->AddItem(groupName, "Gravity", kGravity);

/*
	weaponOBB_.center = worldTransformWepon_.translation_;
	weaponOBB_.size = {1.0f,3.0f,1.0f};
	weaponCollider_.SetOBB(weaponOBB_);
	weaponCollider_.SetIsCollision(false);
	CollisionManager::GetInstance()->PushCollider(&weaponCollider_);

	obbModel_.reset(Model::CreateFromOBJ("cube"));
	worldTtansformOBB_.Initialize();*/
	comboNum_ = 0;

	emitter.count = 1;
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;
	emitter.transform.translate = { 0,0,0 };

	bullets_.clear();
	isDead_ = false;
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
	models_[0].worldTransform_.rotation_ = { 0,0,0 };
	for (HierarchicalAnimation& model : models_) {
		model.worldTransform_.UpdateMatrix();
	}
	direction_ = { 0,0,1.0f };
	directionMatrix_ = MakeIdentity4x4();
	comboNum_ = 0;
	//weaponCollider_.SetIsCollision(false);
	bullets_.clear();
	isDead_ = false;
}

void Player::BehaviorRootInitialize() {
	InitializeFloatingGimmick();
	models_[2].worldTransform_.parent_ = &models_[0].worldTransform_;
	models_[3].worldTransform_.parent_ = &models_[0].worldTransform_;
	models_[2].worldTransform_.translation_.x = -0.5f;
	models_[2].worldTransform_.translation_.y = 1.5f;
	models_[2].worldTransform_.translation_.z = 0.0f;
	models_[3].worldTransform_.translation_.x = 0.5f;
	models_[3].worldTransform_.translation_.y = 1.5f;
	models_[3].worldTransform_.translation_.z = 0.0f;

	models_[2].worldTransform_.rotation_.x = 0.0f;
	models_[2].worldTransform_.rotation_.y = 0.0f;
	models_[3].worldTransform_.rotation_.x = 0.0f;
	models_[3].worldTransform_.rotation_.y = 0.0f;
	comboNum_ = 0;
}

void Player::BehaviorAttackInitialize() {

	worldTransformWepon_.translation_.y = 1.0f;
	worldTransformWepon_.rotation_.x = 0.0f;
	worldTransformWepon_.rotation_.z = -1.57f;

	models_[2].worldTransform_.translation_.x = 0.009f;
	models_[2].worldTransform_.translation_.y = 2.528f;
	models_[2].worldTransform_.translation_.z = -1.20f;
	models_[3].worldTransform_.translation_.x = -0.009f;
	models_[3].worldTransform_.translation_.y = 2.00f;
	models_[3].worldTransform_.translation_.z = -1.20f;
	
	models_[2].worldTransform_.rotation_.x = -1.57f;
	models_[2].worldTransform_.rotation_.y = 0.600f;
	models_[3].worldTransform_.rotation_.x = -1.57f;
	models_[3].worldTransform_.rotation_.y = -0.600f;

	//models_[3].worldTransform_.rotation_.x = -float(std::numbers::pi);
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
	ApplyGlobalVariables();
	if (!isDead_) {


		bullets_.remove_if([](std::unique_ptr<Bullet>& bullet) {
			if (bullet->GetIsDead()) {
				return true;
			}
			return false;
			});
		Input::GetInstance()->GetJoystickState(0, joyState_);
		if (behaviorRequest_) {
			behavior_ = behaviorRequest_.value();
			frameCount_ = 0;
			switch (behavior_) {
			case Player::Behavior::kRoot:
			default:
				BehaviorRootInitialize();
				break;
			case Player::Behavior::kAttack:
				BehaviorAttackInitialize();
				break;
			case Player::Behavior::kDash:
				BehaviorDashInitialize();
				break;
			case Player::Behavior::kJump:
				BehaviorJumpInitialize();
				break;
			}
			behaviorRequest_ = std::nullopt;
		}
		//BehaviorRootUpdate();
		//BehaviorAttackUpdate();
		if (worldTransform_.parent_) {
			if (worldTransform_.translation_.y < 0.0f) {
				worldTransform_.translation_.y = 0.0f;
			}

		}


		switch (behavior_) {
		case Player::Behavior::kRoot:
			BehaviorRootUpdate();
			break;
		case Player::Behavior::kAttack:
			BehaviorAttackUpdate();
			break;
		case Player::Behavior::kDash:
			BehaviorDashUpdate();
			break;
		case Player::Behavior::kJump:
			BehaviorJumpUpdate();
			break;
		}
		/*
		if (worldTransform_.translation_.y < -10.0f) {
			//ReStart();
		}*/
	}
	else {
		if (models_[0].worldTransform_.rotation_.x > -3.14f/2.0f) {
			models_[0].worldTransform_.rotation_.x -= 0.05f;
		}
	}
	// 行列更新
	//worldTransform_.UpdateMatrix();
	worldTransform_.matWorld_ = MakeScaleMatrix(worldTransform_.scale_) * directionMatrix_ * MakeTranslateMatrix(worldTransform_.translation_);
	if (worldTransform_.parent_){
		worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;
	}
	// キャラクタの座標を表示する
	float* slider3[3] = {
	    &worldTransform_.translation_.x, &worldTransform_.translation_.y,
	    &worldTransform_.translation_.z};
	/*
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::SliderFloat3("", *slider3, -100.0f, 100.0f);
	ImGui::SliderInt("Attack", &attackFrame, 1, 240);
	ImGui::SliderInt("Rigd", &rigidityFrame, 1, 240);
	ImGui::SliderInt("End", &endFrame, 1, 240);
	ImGui::End();
#endif // _DEBUG
*/

	obb_.center = worldTransform_.GetWorldPosition();
	obb_.size = { 0.5f,1.0f,0.5f };
	obb_.center.y += obb_.size.y / 2.0f;
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	for (HierarchicalAnimation& model : models_) {
		model.worldTransform_.UpdateMatrix();
	}

	for (std::list<std::unique_ptr<Bullet>>::iterator iterator = bullets_.begin();
		iterator != bullets_.end(); iterator++) {
		(*iterator)->Update();
	}

	preJoyState_ = joyState_;
}

void Player::BehaviorRootUpdate()
{
	// ゲームパッドの状態をえる
	XINPUT_STATE joyState;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) 
	{
		//behavior_ = Behavior::kAttack;
		behaviorRequest_ = Behavior::kAttack;
	}
	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_X))
	{
		//behavior_ = Behavior::kAttack;
		behaviorRequest_ = Behavior::kDash;
	}
	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && 
		worldTransform_.parent_)
	{
		behaviorRequest_ = Behavior::kJump;
	}

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float kCharacterSpeed = 0.3f;

		Vector3 move = {
		    float(joyState.Gamepad.sThumbLX) / SHRT_MAX, 0,
		    float(joyState.Gamepad.sThumbLY) / SHRT_MAX};
		//Matrix4x4 newrotation = DirectionToDIrection({0,0.0f,1.0f}, {0, 0.0f, -1.0f});
		move = Normalize(move) * kCharacterSpeed;
		//Vector3 cameraDirectionYcorection = {0.0f, viewProjection_->matView.m[1][0] * viewProjection_->matView.m[1][0]* viewProjection_->matView.m[1][2], 0.0f};
		Matrix4x4 cameraRotateY = Inverse(viewProjection_->matView);
		//cameraRotateY.m[0][0] = 1;
		cameraRotateY.m[0][1] = 0;
		//cameraRotateY.m[0][2] = 0;

		cameraRotateY.m[1][0] = 0;
		cameraRotateY.m[1][1] = 1;
		cameraRotateY.m[1][2] = 0;


		//cameraRotateY.m[2][0] = 0;
		cameraRotateY.m[2][1] = 0;
		//cameraRotateY.m[2][2] = 1;
		
		cameraRotateY.m[3][0] = 0;
		cameraRotateY.m[3][1] = 0;
		cameraRotateY.m[3][2] = 0;
		//cameraRotateY = Inverse(cameraRotateY);
		move = Transform(move, cameraRotateY);
		if (joyState.Gamepad.sThumbLX != 0 || joyState.Gamepad.sThumbLY != 0) {
			//worldTransform_.rotation_.y = std::atan2(move.x, move.z);
			Matrix4x4 newDirection = DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(move));
			directionMatrix_ = newDirection;
			//worldTransform_.matWorld_ *= newDirection;
			//worldTransform_.rotation_.y = newDirection.m[1][0] * newDirection.m[1][1] * newDirection.m[1][2];
			direction_ = move;
		}
		if (target_) {
			Vector3 toTarget = target_->GetWorldPosition() - worldTransform_.GetWorldPosition();
			toTarget.y = 0;
			directionMatrix_=DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(toTarget));
		}
		worldTransform_.translation_ += move;
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
	}
	if (!worldTransform_.parent_) {
		velocity_ += kGravity;
	}
	worldTransform_.translation_ += velocity_;
	UpdateFloatingGimmick();
}

void Player::BehaviorAttackUpdate()
{
	static float weponRotateEnd = 3.14f;

	static float frontLength = 5.0f;
	
	
	if (frameCount_ == endFrame) {
		// behavior_ = Behavior::kRoot;
		//weaponCollider_.SetIsCollision(false);
		behaviorRequest_ = Behavior::kRoot;
	}
	
	if (target_) {
		Vector3 toTarget = target_->GetWorldPosition() - worldTransform_.GetWorldPosition();
		if (frameCount_ == 0) {
			//shot
			std::unique_ptr<Bullet> bullet;
			bullet.reset(new Bullet());
			bullet->Initialize();
			bullet->SetPosition(models_[1].worldTransform_.GetWorldPosition());
			bullet->SetModel(modelBullet_);
			Vector3 velocity = Normalize(toTarget) * 1.5f;
			bullet->SetVelocity(velocity);
			bullet->SetParticle(particle_);
			bullets_.push_back(std::move(bullet));
			fire_();
		}
		toTarget.y = 0;
		directionMatrix_ = DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(toTarget));
	}

	frameCount_++;
}

void Player::BehaviorDashUpdate() {
	//const float kCharacterSpeed = 0.3f;

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
	if (frameCount_ >= dashLength_) {
		behaviorRequest_ = Behavior::kRoot;
	}
	frameCount_++;
}

void Player::BehaviorJumpUpdate() {
	velocity_ = kJumpVelocity * directionMatrix_;
	behaviorRequest_ = Behavior::kRoot;
}

void Player::Draw(const ViewProjection& viewProjection) {
	//model_->Draw(worldTransform_, viewProjection);
	for (HierarchicalAnimation& model : models_)
	{
		model.model_->Draw(model.worldTransform_, viewProjection);
	}
	for (std::list<std::unique_ptr<Bullet>>::iterator iterator = bullets_.begin();
		iterator != bullets_.end(); iterator++) {
		(*iterator)->Draw(viewProjection);
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
	models_[0].worldTransform_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude;
	models_[2].worldTransform_.rotation_.x = std::cos(floatingParameter_) * floatingAmplitude;
	models_[3].worldTransform_.rotation_.x = std::cos(floatingParameter_) * floatingAmplitude;

	/*
	#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::SliderFloat3("Head Transform", &models_[1].worldTransform_.translation_.x,-10.0f,10.0f);
	ImGui::SliderFloat3(
	    "ArmL Transform", &models_[2].worldTransform_.translation_.x, -10.0f, 10.0f);
	ImGui::SliderFloat3(
	    "ArmR Transform", &models_[3].worldTransform_.translation_.x, -10.0f, 10.0f);
	ImGui::SliderInt("Period", (reinterpret_cast<int*>(&period)), 1, 180);
	ImGui::SliderFloat("Ampritude", &floatingAmplitude,0.0f,10.0f);
	ImGui::End();
#endif // _DEBUG
*/
}

void Player::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	models_[1].worldTransform_.translation_ = globalVariables->GetVector3Value(groupName, "Head Translation");
	models_[2].worldTransform_.translation_ =
	    globalVariables->GetVector3Value(groupName, "ArmL Translation");
	models_[3].worldTransform_.translation_ =
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
	}
}