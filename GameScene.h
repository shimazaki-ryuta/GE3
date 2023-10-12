#pragma once
#include "Vector2.h"
#include "Sprite.h"

#include <sstream>
#include <vector>

#include "Input.h"

#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Model.h"

#include "CommonFiles/DirectXCommon.h"

#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "Ground.h"
#include "FollowCamera.h"

class GameScene
{
public:
	struct Transforms
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	GameScene();
	~GameScene();
	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw3D();
	void Draw2D();

	

private:
	
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;


	struct Transforms cameraTransform { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f } };

	//Model model_;

	//WorldTransform worldTransformObj_;
	//WorldTransform worldTransformObj2_;
	//ViewProjection viewProjection_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 自キャラ
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> modelPlayerBody_;
	std::unique_ptr<Model> modelPlayerHead_;
	std::unique_ptr<Model> modelPlayerL_arm_;
	std::unique_ptr<Model> modelPlayerR_arm_;
	std::unique_ptr<Model> modelWepon_;

	//敵
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<Model> modelEnemyBody_;
	std::unique_ptr<Model> modelEnemyWheel_;

	bool isDebugCameraActive_ = false;
	//DebugCamera* debugCamera_ = nullptr;

	std::unique_ptr<Skydome> skydome_;
	Model* modelSkydome_ = nullptr;

	std::unique_ptr<Ground> ground_;
	Model* modelGround_ = nullptr;

	std::unique_ptr<FollowCamera> followCamera_;

};

