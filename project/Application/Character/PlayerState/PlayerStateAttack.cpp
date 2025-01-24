#include "PlayerStateAttack.h"
#include "PlayerStateFactory.h"
#include "../Player.h"
static int startFrame = 0;
static int endFrame = 40;
void PlayerStateAttack::Initialize() {
	player_->BehaviorAttackInitialize();
}

void PlayerStateAttack::Update() {

	//待機状態に戻る
	if (frameCount_ == endFrame) {
		//敵に向けて射撃する
		player_->Shot();
		player_->ChangeState("ROOT");
		return;
	}

	frameCount_++;
}