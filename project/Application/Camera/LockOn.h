#pragma once
#include "3D/WorldTransform.h"
#include "3D/ViewProjection.h"
#include "Input.h"
#include "Character/Enemy.h"
#include <memory>
#include <list>
#include "TextureManager.h"
#include "2D/Sprite.h"

//敵のロックオン処理

class LockOn
{
public:
	void Initialize();
	void Update(Enemy* enemies,ViewProjection& viewProjection);
	void Draw();

	//対象を探す
	void Search(Enemy* enemies, ViewProjection& viewProjection);
	
	//ターゲット対象を変えす(対象なければ仮対象)
	WorldTransform* GetTarget();

	//Getter/Setter
	bool isInnerCamera(const Vector3& );
	bool IsLockOn() { return isLockOn_; };
	bool IsForcus() { return isForcus_; };
private:
	uint32_t textureHandle_;
	std::unique_ptr<Sprite> AnchorSprite_;
	Enemy* target_;
	bool isLockOn_;
	bool isAutoLock_;
	bool isForcus_;
	XINPUT_STATE preJoyState_;

	WorldTransform notTargetWorldTransform_;
};

