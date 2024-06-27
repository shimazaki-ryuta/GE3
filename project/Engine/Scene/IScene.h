#pragma once
#include "Sprite.h"
#include "Scene/SceneLoader.h"
#include "GameObject/GameObject.h"

class IScene
{
public:

	virtual void Initialize(DirectXCommon* dxCommon) =0;
	virtual void Update() {};
	virtual void Draw3D() {};
	virtual void Draw2D() {};

	virtual ~IScene() {};

protected:

};
