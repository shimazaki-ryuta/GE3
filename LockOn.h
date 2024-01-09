#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
#include "Enemy.h"
#include "Player2.h"
#include <memory>
#include <list>
#include "TextureManager.h"
#include "Sprite.h"
class LockOn
{
public:
	void Initialize();
	void Update(Player2* enemies,ViewProjection& viewProjection);
	void Draw();

	void Search(Player2* enemies, ViewProjection& viewProjection);
	bool IsLockOn() { return isLockOn_; };
	bool IsForcus() { return isForcus_; };
	//WorldTransform* GetTarget() { return target_->GetWorldTransform(); };
	WorldTransform* GetTarget();
	bool isInnerCamera(const Vector3& );
	//void LockChange(std::list<std::unique_ptr<Enemy>>& enemies, ViewProjection& viewProjection,int LR);
private:
	uint32_t textureHandle_;
	std::unique_ptr<Sprite> AnchorSprite_;
	Player2* target_;
	bool isLockOn_;
	bool isAutoLock_;
	bool isForcus_;
	XINPUT_STATE preJoyState_;

	WorldTransform notTargetWorldTransform_;
};

