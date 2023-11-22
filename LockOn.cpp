#include "LockOn.h"
#include "Input.h"
#include "matrixfunction.h"
#include "VectorFunction.h"
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

	if (isLockOn_ && target_->IsDead()) {
		isLockOn_ = false;
	}



	preJoyState_ = joyState;
}

void LockOn::Search(std::list<std::unique_ptr<Enemy>>& enemies, ViewProjection& viewProjection) {
	
	
	Enemy* target;
	isLockOn_ = false;
	if (!enemies.empty()) {
		std::list<std::unique_ptr<Enemy>>::iterator enemy = enemies.begin();
		target = (enemy)->get();
	

		for (enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
			Vector3 targetPosition = target->GetWorldTransform()->GetWorldPosition();
			Vector3 newEnemyPosition = enemy->get()->GetWorldTransform()->GetWorldPosition();
			Vector3 innerCameraPos = newEnemyPosition;
			innerCameraPos = innerCameraPos * viewProjection.matView * viewProjection.matProjection;
			float oldLength = Length(targetPosition - viewProjection.translation_);
			float newLength = Length(newEnemyPosition - viewProjection.translation_);
			if (std::abs(innerCameraPos.x) <= 1.0f && std::abs(innerCameraPos.y) <= 1.0f && innerCameraPos.z > 0.0f &&
				newLength <= oldLength && !enemy->get()->IsDead()) {
				target = enemy->get();
				target_ = target;
				isLockOn_ = true;
			}
		}
	}
	
}

void LockOn::Draw() {

}