#include "PlayerStateJump.h"
#include "PlayerStateFactory.h"
#include "../Player.h"
void PlayerStateJump::Initialize() {
	player_->BehaviorJumpInitialize();
}

void PlayerStateJump::Update() {

	player_->SetVelocity(kJumpVelocity * player_->GetDirectionMatrix());
	player_->ChangeState("ROOT");
}