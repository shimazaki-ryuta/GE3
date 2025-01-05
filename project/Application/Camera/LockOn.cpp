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

void LockOn::Update(Enemy* enemies, ViewProjection& viewProjection) {
	

	Search(enemies, viewProjection);
	isForcus_ = false;
	
	//ロックオン対象の補正
	if (isLockOn_) {
		Vector3 targetPos = target_->GetWorldTransformBody()->GetWorldPosition();
		targetPos = targetPos *viewProjection.matView * viewProjection.matProjection;
		targetPos = targetPos * MakeViewportMatrix(0, 0, viewProjection.width, viewProjection.height, 0, 1);
		AnchorSprite_->SetPosition({ targetPos.x ,targetPos.y });
	}
	else {
		//仮想ターゲットを移動
		Matrix4x4 cameraRotate = Inverse(viewProjection.matView);
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
	//一点がカメラ内に入ってるか判定する
	if (std::abs(vector.x) <=1.0f && std::abs(vector.y) <= 1.0f && vector.z > 0.0f){
		return true;
	}
	return false;
}

void LockOn::Search(Enemy* enemy, ViewProjection& viewProjection) {
	
	
	Enemy* target;
	isLockOn_ = false;
	//ターゲットが存在したら判定を取る
	if (enemy) {
		//ターゲットの座標をスクリーン座標に変換
		Vector3 newEnemyPosition = enemy->GetWorldTransformBody()->GetWorldPosition();
		Vector3 innerCameraPos = newEnemyPosition;
		innerCameraPos = innerCameraPos * viewProjection.matView * viewProjection.matProjection;
			
		bool modeCheck = true;
		//カメラ内か
		if (std::abs(innerCameraPos.x) <= 1.0f && std::abs(innerCameraPos.y) <= 1.0f && innerCameraPos.z > 0.0f &&
			modeCheck) {
			//新しいターゲットをセット
			target = enemy;
			target_ = target;
			isLockOn_ = true;
		}
		
	}
	
}

void LockOn::Draw() {
	AnchorSprite_->Draw();
}