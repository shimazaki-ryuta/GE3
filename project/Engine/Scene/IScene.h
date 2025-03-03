#pragma once
#include "2D/Sprite.h"
#include "Scene/SceneLoader.h"
#include "GameObject/GameObject.h"


using namespace MyEngine;
//シーン基底クラス

class IScene
{
public:

	//初期化
	virtual void Initialize(DirectXCommon* dxCommon) { dxCommon_ = dxCommon; };
	//更新
	virtual void Update() {};
	//3D描画
	virtual void Draw3D() {};
	//2D描画
	virtual void Draw2D() {};

	virtual ~IScene() {};

protected:
	DirectXCommon* dxCommon_ = nullptr;
};
