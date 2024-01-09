#include "FollowCamera.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "Input.h"
#include <algorithm>
#include <numbers>
#include "GlobalVariables.h"
#include "RandomEngine.h"
void FollowCamera::Initialize()
{
	viewProjection_.Initialize(); 
	GlobalVariables* grovalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	grovalVariables->CreateGroup(groupName);
	grovalVariables->AddItem(groupName, "CameraDelay", cameraDelay_);
}
void FollowCamera::Update() 
{
	ApplyGlobalVariables();
	// ゲームパッドの状態をえる
	XINPUT_STATE joyState;

	if (Input::GetJoystickState(0, joyState)) 
	{

		const float rotateSpeed = 0.1f;

		viewProjection_.rotation_.y += float(joyState.Gamepad.sThumbRX) / SHRT_MAX * rotateSpeed;
		viewProjection_.rotation_.x -= float(joyState.Gamepad.sThumbRY) / SHRT_MAX * rotateSpeed;
		viewProjection_.rotation_.x = std::clamp(viewProjection_.rotation_.x, float(-std::numbers::pi)/2.0f+0.1f, float(std::numbers::pi)/2.0f-0.1f);
	}

	Matrix4x4 rotateMatrix1 = MakeRotateMatrix(viewProjection_.rotation_);
	rotateMatrix3_ = MakeIdentity4x4();
	if (target_)
	{
		Vector3 offset = {0.0f,3.0f,-20.0f};

		
		if (lockOnTarget_ != nullptr) {
			viewProjection_.rotation_.x = 0;
			viewProjection_.rotation_.y = 0;
			rotateMatrix1 = MakeRotateMatrix(viewProjection_.rotation_);

			//Matrix4x4 rotateMatrix2;
			Vector3 targetYMask = target_->GetWorldPosition();
			targetYMask.y = 0;
			Vector3 lockOnYMask = lockOnTarget_->GetWorldPosition();
			lockOnYMask.y = 0;
			//lockOnYMask.z = 1;
			Vector3 toLockOn = lockOnYMask - targetYMask;
			rotateMatrix2_ = DirectionToDIrection(Normalize({ 0,0.0f,1.0f}),Normalize(toLockOn));
			toLockOn = lockOnTarget_->GetWorldPosition() - target_->GetWorldPosition();
			//toLockOn.y /= Length(toLockOn);
			//toLockOn.x = 0.0f;
			//toLockOn.z = 1.0f;
			//toLockOn.y /= Length(toLockOn);
			//toLockOn.y = -std::abs(toLockOn.y);

			rotateMatrix3_ = DirectionToDIrection(Normalize({ toLockOn.x,0.0f,toLockOn.z }), Normalize(toLockOn));
			//rotateMatrix2_.m[0][1] = 0;

			//rotateMatrix2_.m[1][0] = 0;
			//rotateMatrix2_.m[1][1] = 1;
			//rotateMatrix2_.m[1][2] = 0;
			
			//rotateMatrix2_.m[2][1] = 0;
			
			Vector3 axis;
			for (int32_t index = 0; index < 3; index++) {
				axis = { rotateMatrix3_.m[index][0],rotateMatrix3_.m[index][1],rotateMatrix3_.m[index][2] };
				float length = Length(axis);
				rotateMatrix3_.m[index][0] /= length;
				rotateMatrix3_.m[index][1] /= length;
				rotateMatrix3_.m[index][2] /= length;
			}

			//rotateMatrix_ = rotateMatrix1* rotateMatrix2_;
		}
		rotateMatrix_ = rotateMatrix1 * rotateMatrix2_ * rotateMatrix3_;
		offset = TransformNormal(offset,rotateMatrix_);

		interTargert_ = Lerp(interTargert_,target_->GetWorldPosition(),cameraDelay_);

		viewProjection_.translation_ = interTargert_ + offset;
	}

	if (isShake_) {
		viewProjection_.translation_.x += RandomEngine::GetInstance()->GetRandom(-shakeForce_ / 2.0f, shakeForce_ / 2.0f);
		viewProjection_.translation_.y += RandomEngine::GetInstance()->GetRandom(-shakeForce_ / 2.0f, shakeForce_ / 2.0f);
		viewProjection_.translation_.z += RandomEngine::GetInstance()->GetRandom(-shakeForce_ / 2.0f, shakeForce_ / 2.0f);
		shakeForce_ *= 0.8f;
		if (shakeForce_ <= 0.1f) {
			isShake_ = false;
		}
	}

	//viewProjection_.UpdateMatrix();
	viewProjection_.matView = Inverse(rotateMatrix_* MakeTranslateMatrix(viewProjection_.translation_));
	viewProjection_.matProjection = MakePerspectiveFovMatrix(viewProjection_.fovAngleY, viewProjection_.width / viewProjection_.height, viewProjection_.nearZ, viewProjection_.farZ);
}

void FollowCamera::Reset() {
	rotateMatrix2_ = MakeIdentity4x4();
	viewProjection_.rotation_ = {0,0,0};
	if (target_)
	{
		Vector3 offset = { 0.0f,2.0f,-20.0f };

		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_.rotation_);

		offset = TransformNormal(offset, rotateMatrix);

		viewProjection_.translation_ = target_->GetWorldPosition() + offset;
		interTargert_ = target_->GetWorldPosition();
	}

	viewProjection_.UpdateMatrix();
}

void FollowCamera::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	cameraDelay_ = globalVariables->GetFloatValue(groupName, "CameraDelay");
}
void FollowCamera::Shake() {
	isShake_ = true;
	shakeForce_ = kShakeForce_;
}