#include "FollowCamera.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "Input.h"
#include <algorithm>
#include <numbers>
void FollowCamera::Initialize()
{
	viewProjection_.Initialize(); 

}
void FollowCamera::Update() 
{
	// ゲームパッドの状態をえる
	XINPUT_STATE joyState;

	if (Input::GetJoystickState(0, joyState)) 
	{

		const float rotateSpeed = 0.1f;

		viewProjection_.rotation_.y += float(joyState.Gamepad.sThumbRX) / SHRT_MAX * rotateSpeed;
		viewProjection_.rotation_.x -= float(joyState.Gamepad.sThumbRY) / SHRT_MAX * rotateSpeed;
		viewProjection_.rotation_.x = std::clamp(viewProjection_.rotation_.x, float(-std::numbers::pi)/2.0f+0.1f, float(std::numbers::pi)/2.0f-0.1f);
	}

	if (target_)
	{
		Vector3 offset = {0.0f,2.0f,-20.0f};

		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_.rotation_);
		
		offset = TransformNormal(offset,rotateMatrix);

		viewProjection_.translation_ = target_->GetWorldPosition() + offset;
	}

	viewProjection_.UpdateMatrix();
}
