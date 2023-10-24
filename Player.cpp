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

static int startFrame = 0;
static int endFrame = 60;
static int rigidityFrame = 30;
static int attackFrame = 20;


void Player::Initialize(const std::vector<HierarchicalAnimation>& models) {
	GlobalVariables* grovalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	grovalVariables->CreateGroup(groupName);
	//grovalVariables->AddItem(groupName, "Test",90);
	//grovalVariables->AddItem(groupName, "Test2", 5.0f);
	//grovalVariables->AddItem(groupName, "Test3", Vector3{1.0f,2.0f,3.0f});


	//assert(model);
	BaseCharacter::Initialize(models);
	obb_.center = worldTransform_.translation_;
	obb_.size = {1.0f,1.0f,1.0f};
	obb_.center.y += obb_.size.y / 2.0f;
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	input_ = Input::GetInstance();

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
	
}

void Player::ReStart() {
	behaviorRequest_ = Behavior::kRoot;
	worldTransform_.parent_ = nullptr;
	worldTransform_.rotation_ = {0.0f,0.0f,0.0f};
	worldTransform_.translation_ = {0.0f,0.0f,0.0f};
	worldTransform_.Initialize();
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
	models_[2].worldTransform_.parent_ = &worldTransformWepon_;
	models_[3].worldTransform_.parent_ = &worldTransformWepon_;
	attackBehavior_ = AttackBehavior::kPre;
}

void Player::Update() {
	ApplyGlobalVariables();
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
		}
		behaviorRequest_ = std::nullopt;
	}
	//BehaviorRootUpdate();
	//BehaviorAttackUpdate();
	switch (behavior_) {
	case Player::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	if (worldTransform_.parent_) {
		worldTransform_.translation_.y = 0.0f;
	}
	else {
		worldTransform_.translation_.y -= 0.4f;
	}

	if (worldTransform_.translation_.y < -10.0f) {
		ReStart();
	}

	// 行列更新
	worldTransform_.UpdateMatrix();

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
	obb_.size = { 1.0f,1.0f,1.0f };
	obb_.center.y += obb_.size.y / 2.0f;
	SetOridentatios(obb_, MakeRotateMatrix(worldTransform_.rotation_));
	for (HierarchicalAnimation& model : models_) {
		model.worldTransform_.UpdateMatrix();
	}
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

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float kCharacterSpeed = 0.3f;

		Vector3 move = {
		    float(joyState.Gamepad.sThumbLX) / SHRT_MAX, 0,
		    float(joyState.Gamepad.sThumbLY) / SHRT_MAX};

		move = Normalize(move) * kCharacterSpeed;
		Vector3 cameraDirectionYcorection = {0.0f, viewProjection_->rotation_.y, 0.0f};
		move = Transform(move, MakeRotateMatrix(cameraDirectionYcorection));
		if (joyState.Gamepad.sThumbLX != 0 || joyState.Gamepad.sThumbLY != 0) {
			worldTransform_.rotation_.y = std::atan2(move.x, move.z);
		}
		worldTransform_.translation_ += move;
	}
	UpdateFloatingGimmick();
}

void Player::BehaviorAttackUpdate()
{
	static float weponRotateEnd = 3.14f;
	
	static float frontLength = 5.0f;
	/*
	if (frameCount_ < startFrame) {
		Vector3 move = {0.0f, 0.0f, frontLength / float(startFrame)};
		worldTransform_.translation_ +=
		    Transform(move, MakeRotateMatrix(worldTransform_.rotation_));
	}
	if (frameCount_ >= startFrame)
	{
		if (worldTransformWepon_.rotation_.x <= weponRotateEnd) {
			worldTransformWepon_.rotation_.x += 0.1f;
			// worldTransformWepon_.rotation_.x = 0.0f;
			// behavior_ = Behavior::kRoot;
		}
		attackBehavior_ = AttackBehavior::kAttack;
	}*/
	if (frameCount_ == attackFrame)
	{
		attackBehavior_ = AttackBehavior::kAttack;
	}
	if (frameCount_ == rigidityFrame) {
		attackBehavior_ = AttackBehavior::kEnd;
	}
	if (frameCount_ == endFrame) {
		// behavior_ = Behavior::kRoot;
		behaviorRequest_ = Behavior::kRoot;
	}
	Vector3 move = {0.0f, 0.0f, frontLength / float(attackFrame)};
	
	move = Transform(move, MakeRotateMatrix(worldTransform_.rotation_));

	switch (attackBehavior_) {
	case Player::AttackBehavior::kPre:

		worldTransform_.translation_ += move;
		break;
	case Player::AttackBehavior::kAttack:
		worldTransformWepon_.rotation_.x += weponRotateEnd / float(rigidityFrame - attackFrame);
		break;

	}
	/*
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::SliderFloat3(
	    "ArmL Transform", &models_[2].worldTransform_.translation_.x, -10.0f, 10.0f);
	ImGui::SliderFloat3(
	    "ArmL Rotate", &models_[2].worldTransform_.rotation_.x, -3.0f, 3.0f);
	ImGui::SliderFloat3(
	    "ArmR Transform", &models_[3].worldTransform_.translation_.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("ArmR Rotate", &models_[3].worldTransform_.rotation_.x, -3.0f, 3.0f);
	ImGui::SliderInt("Attack", &attackFrame, 1, 240);
	ImGui::SliderInt("Rigd", &rigidityFrame, 1, 240);
	ImGui::SliderInt("End", &endFrame, 1, 240);
	ImGui::SliderFloat("wepon", &weponRotateEnd, 0.0f, 2.0f);
	ImGui::SliderFloat("front", &frontLength, 0.0f, 10.0f);
	ImGui::End();
#endif // _DEBUG
*/
	worldTransformWepon_.UpdateMatrix();
	frameCount_++;
}

void Player::Draw(const ViewProjection& viewProjection) {
	//model_->Draw(worldTransform_, viewProjection);
	for (HierarchicalAnimation& model : models_)
	{
		model.model_->Draw(model.worldTransform_, viewProjection);
	}
	if (behavior_ == Player::Behavior::kAttack)
	{
		modelWepon_->Draw(worldTransformWepon_, viewProjection);
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
}

void Player::OnCollision(WorldTransform& parent)
{
	if (worldTransform_.parent_ != &parent) {
		Matrix4x4 rocal = worldTransform_.matWorld_ * (Inverse(parent.matWorld_));
		worldTransform_.translation_.x = rocal.m[3][0];
		worldTransform_.translation_.y = rocal.m[3][1];
		worldTransform_.translation_.z = rocal.m[3][2];

		worldTransform_.parent_ = &parent;
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