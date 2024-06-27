#pragma once
#include "Scene/IScene.h"

#include "Vector2.h"

#include <sstream>
#include <vector>

#include "Input.h"

#include "ViewProjection.h"
#include "WorldTransform.h"

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
#include "Engine/Animation.h"
#include "Engine/SkyBox.h"

class GameScene : public IScene
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
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw3D() override;
	void Draw2D() override;
	

private:
	
	Microsoft::WRL::ComPtr<ID3D12Resource> directinalLightResource;
	DirectionalLight* directinalLightData = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;
	PointLight* pointLightData = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource;
	SpotLight* spotLightData = nullptr;


	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	std::unique_ptr<DebugCamera> debugCamera_;
	bool isDebugCameraActive_ = false;

	struct Transforms cameraTransform { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f } };

	// ビュープロジェクション
	ViewProjection viewProjection_;
//1フレーム前の入力情報
	XINPUT_STATE preJoyState_;
	
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	uint32_t pointLightMax_=32;
	int colorPhase_;
	float color_;

	float grayScaleValue_;//グレースケールの強さ
	std::unique_ptr<Animation> bulletAnimation_;

	std::unique_ptr<SkyBox> skyBox_;
	WorldTransform worldTransformSkyBox_;

	std::unique_ptr<Ground> ground_;
	Model* modelGround_ = nullptr;

	std::unique_ptr<SceneLoader> sceneLoader_;
	std::map<std::string, std::unique_ptr<Model>> modelList_;
	std::vector<std::unique_ptr<GameObject>> objects_;
};

