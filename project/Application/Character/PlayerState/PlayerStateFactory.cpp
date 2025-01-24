#include "PlayerStateFactory.h"
#include "PlayerStateRoot.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDash.h"
#include "PlayerStateJump.h"
BasePlayerState* PlayerStateFactory::CreateState(const std::string& stateName) {
	BasePlayerState* state = nullptr;
	if (stateName == "ROOT") {
		state = new PlayerStateRoot;
	}
	else if (stateName == "ATTACK") {
		state = new PlayerStateAttack;
	}
	else if (stateName == "DASH") {
		state = new PlayerStateDash;
	}
	else if (stateName == "JUMP") {
		state = new PlayerStateJump;
	}
	return state;
}