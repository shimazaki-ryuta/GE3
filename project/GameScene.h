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
#include "Primitive3D.h"
#include "CommonFiles/DirectXCommon.h"

#include "DebugCamera.h"

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

	struct AccelerationField
	{
		Vector3 acceleration;
		AABB area;
	};

	struct DirectionalLight
	{
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

	struct PointLight
	{
		Vector4 color;
		Vector3 position;
		float intensity;
		float radius;
		float decay;
		int32_t isUse_;
		//float padding[2];
	};
	struct SpotLight
	{
		Vector4 color;
		Vector3 position;
		float intensity;
		Vector3 direction;
		float distance;
		float decay;
		float cosAngle;
		float padding[2];
	};

	GameScene();
	~GameScene();
	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw3D();
	void Draw2D();

private:
	
	
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;
	PointLight* pointLightData = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource;
	SpotLight* spotLightData = nullptr;


	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	std::unique_ptr<DebugCamera> debugCamera_;
	struct Transforms cameraTransform { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f } };

	// ビュープロジェクション
	ViewProjection viewProjection_;


	uint32_t uvCheckerTextureHandle_;
	uint32_t monsterTextureHandle_;


	bool isDebugCameraActive_ = false;
	//DebugCamera* debugCamera_ = nullptr;

	std::unique_ptr<Skydome> skydome_;
	Model* modelSkydome_ = nullptr;

	std::unique_ptr<Ground> ground_;
	Model* modelGround_ = nullptr;

	std::unique_ptr<Model> sphere_;

	//std::unique_ptr<Primitive3D> sphere_;

	std::unique_ptr<Particle> particle;

	std::unique_ptr<Sprite> sprite_;

	Vector2 spritePosition_;
	Vector2 ancorPoint_;
	float rotate_;
	Vector2 leftTop;
	Vector2 rightDown;

	bool usebillboard;

	Microsoft::WRL::ComPtr<ID3D12Resource> directinalLightResource;
	DirectionalLight* directinalLightData = nullptr;

	WorldTransform worldTransformSphere_;
	float shininess_;

	Vector4 lineColor_ = { 0 };
	Vector3 lineWidth_ = { 1.05f,1.05f,1.05f };

	uint32_t audioHandle_;
	int32_t shadeType_ = 0;
	uint32_t toonShadowTextureHandle_;

	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	uint32_t pointLightMax_ = 32;
	bool isFlipX_=false, isFlipY_ = false;
	Vector3 cameraTransform_;
	Vector3 cameraRotate_;
};

