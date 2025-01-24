#include "PlayerStateRoot.h"
#include "PlayerStateFactory.h"
#include "../Player.h"
#include "VectorFunction.h"
void PlayerStateRoot::Initialize() {
	player_->BehaviorRootInitialize();
}

void PlayerStateRoot::Update() {

	// ゲームパッドの状態をえる
	XINPUT_STATE joyState;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	//ステート変更
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		player_->ChangeState("ATTACK");
		return;
	}
	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) && !(player_->GetPreJoyState()->Gamepad.wButtons & XINPUT_GAMEPAD_X))
	{
		player_->ChangeState("DASH");
		return;
	}
	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(player_->GetPreJoyState()->Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
		player_->GetIsJump())
	{
		player_->ChangeState("JUMP");
		return;
	}


	//移動処理
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float kCharacterSpeed = 0.3f;

		//カメラ方向から移動向きを補正
		Vector3 move = {
			float(joyState.Gamepad.sThumbLX) / SHRT_MAX, 0,
			float(joyState.Gamepad.sThumbLY) / SHRT_MAX };
		move = Normalize(move) * kCharacterSpeed;
		Matrix4x4 cameraRotateY = Inverse(player_->GetViewProjection()->matView);
		cameraRotateY.m[0][1] = 0;
		cameraRotateY.m[1][0] = 0;
		cameraRotateY.m[1][1] = 1;
		cameraRotateY.m[1][2] = 0;
		cameraRotateY.m[2][1] = 0;
		cameraRotateY.m[3][0] = 0;
		cameraRotateY.m[3][1] = 0;
		cameraRotateY.m[3][2] = 0;
		move = Transform(move, cameraRotateY);

		//移動方向に向きを合わせる
		if (joyState.Gamepad.sThumbLX != 0 || joyState.Gamepad.sThumbLY != 0) {
			Matrix4x4 newDirection = DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(move));
			player_->SetDirectionMatrix(newDirection);
			player_->SetDirection(move);
		}

		//敵の方を向く
		player_->ToTarget();

		//最終的な移動量を足す
		player_->GetWorldTransform()->translation_ += move;

		
	}
	if (!player_->GetWorldTransform()->parent_) {
		player_->SetVelocity(player_->GetVelocity() + kGravity);
	}
	player_->GetWorldTransform()->translation_ += player_->GetVelocity();

	//アニメーション
	player_->UpdateFloatingGimmick();
}