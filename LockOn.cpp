#include "LockOn.h"
#include "Input.h"
#include "matrixfunction.h"
#include "VectorFunction.h"
void LockOn::Initialize() {
	isLockOn_ = false;
	isAutoLock_ = false;
	textureHandle_ = TextureManager::LoadTexture("2DReticle.png");
	//AnchorSprite_.reset(Sprite::Create());
}

void LockOn::Update(std::list<std::unique_ptr<Enemy>>& enemies, ViewProjection& viewProjection) {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) &&
		!(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)) {
		isAutoLock_ = !isAutoLock_;
		if (isAutoLock_) {
			Search(enemies, viewProjection);
			if (isLockOn_) {
				isAutoLock_ = true;
			}
			else {
				isAutoLock_ = false;
			}
		}
		else {
			isLockOn_ = false;
		}
		
	}

	if(isAutoLock_){
		Search(enemies, viewProjection);
		isForcus_ = false;
	}
	else {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) &&
			!(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
			isLockOn_ = !isLockOn_;
			isForcus_ = isLockOn_;
			if (isLockOn_) {
				Search(enemies, viewProjection);
			}
		}

		if (isLockOn_ && target_->GetHP() <= 0) {
			//isLockOn_ = false;
			Search(enemies, viewProjection);
		}
	}

	preJoyState_ = joyState;
}

bool LockOn::isInnerCamera(const Vector3& vector) {
	if (std::abs(vector.x) <=1.0f && std::abs(vector.y) <= 1.0f && vector.z > 0.0f){
		return true;
	}
	return false;
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
			Vector3 oldCameraPos = targetPosition;
			oldCameraPos = oldCameraPos * viewProjection.matView * viewProjection.matProjection;

			float oldLength = Length(targetPosition - viewProjection.translation_);
			float newLength = Length(newEnemyPosition - viewProjection.translation_);
			bool modeCheck;
			if (isAutoLock_) {
				modeCheck = (oldLength  >= newLength) || !isInnerCamera(oldCameraPos);
			}
			else {
				modeCheck = std::abs(innerCameraPos.x) <= std::abs(oldCameraPos.x);
			}
			if (std::abs(innerCameraPos.x) <= 1.0f && std::abs(innerCameraPos.y) <= 1.0f && innerCameraPos.z > 0.0f &&
				modeCheck && enemy->get()->GetHP() > 0) {
				target = enemy->get();
				target_ = target;
				isLockOn_ = true;
			}
		}
	}
	
}

void LockOn::Draw() {

}