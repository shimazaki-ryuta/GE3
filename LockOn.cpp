#include "LockOn.h"
#include "Input.h"
void LockOn::Initialize() {
	isLockOn_ = false;
}

void LockOn::Update(std::list<std::unique_ptr<Enemy>>& enemies, ViewProjection& viewProjection) {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) &&
		!(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		isLockOn_ = !isLockOn_;
		if (isLockOn_) {
			Search(enemies,viewProjection);
		}
	}




	preJoyState_ = joyState;
}

void LockOn::Search(std::list<std::unique_ptr<Enemy>>& enemies, ViewProjection& viewProjection) {
	
	
	/*Enemy* target;
	std::list<std::unique_ptr<Enemy>>::iterator enemy = enemies.begin();
	target = (*enemy).get();
	for (enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
		enemy->get();
		if () {

		}
	}*/
}

void LockOn::Draw() {

}