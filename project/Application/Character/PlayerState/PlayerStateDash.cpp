#include "PlayerStateDash.h"
#include "PlayerStateFactory.h"
#include "../Player.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
void PlayerStateDash::Initialize() {
	player_->BehaviorDashInitialize();
}

void PlayerStateDash::Update() {

	Vector3 move = { 0,0,dashSpeed_ };
	//回避方向を向く
	Matrix4x4 rotate = player_->GetWorldTransform()->matWorld_;
	rotate.m[3][0] = 0;
	rotate.m[3][1] = 0;
	rotate.m[3][2] = 0;
	
	player_->ToTarget();

	//移動方向補正
	move = Transform(move, rotate);
	player_->GetWorldTransform()->translation_ += move;

	//待機状態に戻る
	if (frameCount_ >= dashLength_) {
		player_->ChangeState("ROOT");
	}
	frameCount_++;
}