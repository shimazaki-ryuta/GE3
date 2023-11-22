#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
#include "Enemy.h"
#include <memory>
#include <list>
#include "TextureManager.h"
#include "Sprite.h"
class LockOn
{
public:
	void Initialize();
	void Update(std::list<std::unique_ptr<Enemy>>& enemies,ViewProjection& viewProjection);
	void Draw();

	void Search(std::list<std::unique_ptr<Enemy>>& enemies, ViewProjection& viewProjection);
	bool IsLockOn() { return isLockOn_; };
	WorldTransform* GetTarget() { return target_->GetWorldTransform(); };
private:
	uint32_t txetureHandle_;
	std::unique_ptr<Sprite> AnchorSprite_;
	Enemy* target_;
	bool isLockOn_;
	XINPUT_STATE preJoyState_;
};

