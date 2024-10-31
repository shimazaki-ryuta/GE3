#include "LockOn.h"
#include "Input.h"
#include "matrixfunction.h"
#include "VectorFunction.h"
void LockOn::Initialize() {
	isLockOn_ = false;
	isAutoLock_ = false;
	textureHandle_ = TextureManager::LoadTexture("2DReticle.png");
	AnchorSprite_.reset(Sprite::Create(textureHandle_, {0.0f,0.0f}, {30.0f,30.0f}, {1.0f,1.0f,1.0f,1.0f}));
	notTargetWorldTransform_.Initialize();
}

void LockOn::Update(Player2* enemies, ViewProjection& viewProjection) {
	

	//if(isAutoLock_){
		Search(enemies, viewProjection);
		isForcus_ = false;
	//}
	
	if (isLockOn_) {
		Vector3 targetPos = target_->GetWorldTransformBody()->GetWorldPosition();
		targetPos = targetPos *viewProjection.matView * viewProjection.matProjection;
		targetPos = targetPos * MakeViewportMatrix(0, 0, viewProjection.width, viewProjection.height, 0, 1);
		AnchorSprite_->SetPosition({ targetPos.x ,targetPos.y });
	}
	else {
		Matrix4x4 cameraRotate = Inverse(viewProjection.matView);
		//cameraRotate.m[3][0] = 0;
		//cameraRotate.m[3][1] = 0;
		//cameraRotate.m[3][2] = 0;
		notTargetWorldTransform_.translation_ = Transform({0,0,200.0f},cameraRotate);
		notTargetWorldTransform_.UpdateMatrix();
		Vector3 targetPos = notTargetWorldTransform_.GetWorldPosition();
		targetPos = targetPos * viewProjection.matView * viewProjection.matProjection;
		targetPos = targetPos * MakeViewportMatrix(0, 0, viewProjection.width, viewProjection.height, 0, 1);
		AnchorSprite_->SetPosition({ targetPos.x ,targetPos.y });
	}
}

WorldTransform* LockOn::GetTarget() {
	if (isLockOn_) {
		return target_->GetWorldTransformBody();
	}
	else {
		return &notTargetWorldTransform_;
	}
}

bool LockOn::isInnerCamera(const Vector3& vector) {
	if (std::abs(vector.x) <=1.0f && std::abs(vector.y) <= 1.0f && vector.z > 0.0f){
		return true;
	}
	return false;
}

void LockOn::Search(Player2* enemy, ViewProjection& viewProjection) {
	
	
	Player2* target;
	isLockOn_ = false;
	if (enemy) {
	

			//Vector3 targetPosition = target->GetWorldTransform()->GetWorldPosition();
			Vector3 newEnemyPosition = enemy->GetWorldTransformBody()->GetWorldPosition();
			Vector3 innerCameraPos = newEnemyPosition;
			innerCameraPos = innerCameraPos * viewProjection.matView * viewProjection.matProjection;
			
			//float oldLength = Length(targetPosition - viewProjection.translation_);
			float newLength = Length(newEnemyPosition - viewProjection.translation_);
			bool modeCheck = true;
			//modeCheck =  isInnerCamera(innerCameraPos)) || !isInnerCamera(oldCameraPos);
			if (std::abs(innerCameraPos.x) <= 1.0f && std::abs(innerCameraPos.y) <= 1.0f && innerCameraPos.z > 0.0f &&
				modeCheck) {
				target = enemy;
				target_ = target;
				isLockOn_ = true;
			}
		
	}
	
}

void LockOn::Draw() {
	//if (isLockOn_) {
		AnchorSprite_->Draw();
	//}
}