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
#include "Particle.h"

#include "CommonFiles/DirectXCommon.h"

#include "Player.h"
#include "Player2.h"
#include "PlayerAI.h"
#include "Enemy.h"
#include "Skydome.h"
#include "Ground.h"
#include "FollowCamera.h"
#include "flooar.h"
#include "Goal.h"
#include "MovingFlooar.h"
#include <array>
#include <memory>
#include "LockOn.h"
class GameScene
{
public:
	struct Transforms
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	struct DirectionalLight
	{
		Vector4 color;
		Vector3 direction;
		float intensity;
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

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 自キャラ
	std::unique_ptr<Player> player_;

	std::unique_ptr<Model> modelPlayerBody_;
	std::unique_ptr<Model> modelPlayerHead_;
	std::unique_ptr<Model> modelPlayerL_arm_;
	std::unique_ptr<Model> modelPlayerR_arm_;
	std::unique_ptr<Model> modelWepon_;

	
	std::unique_ptr<Player2> player2_;

	bool isDebugCameraActive_ = false;
	//DebugCamera* debugCamera_ = nullptr;

	std::unique_ptr<Skydome> skydome_;
	Model* modelSkydome_ = nullptr;

	std::unique_ptr<Ground> ground_;
	Model* modelGround_ = nullptr;

	std::array<std::unique_ptr<Flooar>,size_t(5)> flooars_;
	//std::unique_ptr<Flooar> flooar_;

	//std::unique_ptr<Goal> goal_;

	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<LockOn> lockOn_;

	Microsoft::WRL::ComPtr<ID3D12Resource> directinalLightResource;
	DirectionalLight* directinalLightData = nullptr;

	std::unique_ptr<Particle> particle;

	std::unique_ptr<Model> modelBullet_;

	std::unique_ptr<PlayerAI> ai_;

	uint32_t backTextureHandle_;
	std::unique_ptr<Sprite> backSprite_;
	bool isEnd_;
	uint32_t endTextureHandle_[2];
	std::unique_ptr<Sprite> endSprite_;

	bool isIngame_;
	//1フレーム前の入力情報
	XINPUT_STATE preJoyState_;
	uint32_t pressATextureHandle_;
	std::unique_ptr<Sprite> pressASprite_;
	int32_t endCount_;
	bool isButtonDraw_;
	int32_t buttonCount_;
	uint32_t shotTextureHandle_;
	uint32_t dashTextureHandle_;
	uint32_t jumpTextureHandle_;
	std::unique_ptr<Sprite> shotSprite_;
	std::unique_ptr<Sprite> dashSprite_;
	std::unique_ptr<Sprite> jumpSprite_;
	uint32_t titleTextureHandle_;
	std::unique_ptr<Sprite> titleSprite_;
};

