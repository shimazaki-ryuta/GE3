#pragma once
#include "2D/Sprite.h"
#include "Scene/SceneLoader.h"
#include "GameObject/GameObject.h"

class IScene
{
public:

	virtual void Initialize(DirectXCommon* dxCommon) { dxCommon_ = dxCommon; };
	virtual void Update() {};
	virtual void Draw3D() {};
	virtual void Draw2D() {};

	virtual ~IScene() {};

protected:
	DirectXCommon* dxCommon_ = nullptr;
};
