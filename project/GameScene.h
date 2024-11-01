
#pragma once
#include "Scene/IScene.h"

#include "Vector2.h"

#include <sstream>
#include <vector>

#include "Input.h"

#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"

#include "DebugCamera.h"

#include "Character/Player.h"
#include "Character/Player2.h"
#include "Character/PlayerAI.h"
//#include "3D/Skydome.h"
#include "Ground.h"
#include "Camera/FollowCamera.h"
#include "GameObject/flooar.h"
//#include "3D/Goal.h"
#include "GameObject/MovingFlooar.h"
#include <array>
#include <memory>
#include "Camera/LockOn.h"
#include "3D/Animation.h"
#include "3D/SkyBox.h"

#include "Terrain/Terrain.h"

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

	void CreateLight();

	//ステート関数
	void Idle();
	void Play();
	void End();

	//ステート
	std::function<void(void)> state_;

	void Fade();

	//Blender同期
	void FromBlenderUpdate();

	Microsoft::WRL::ComPtr<ID3D12Resource> directinalLightResource;
	DirectionalLight* directinalLightData = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;
	PointLight* pointLightData = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource;
	SpotLight* spotLightData = nullptr;

	Input* input_ = nullptr;

	std::unique_ptr<DebugCamera> debugCamera_;
	bool isDebugCameraActive_ = false;

	struct Transforms cameraTransform { { 1.0f, 1.0f, 1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f } };

	// ビュープロジェクション
	ViewProjection viewProjection_;
	//1フレーム前の入力情報
	XINPUT_STATE preJoyState_;

	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	uint32_t pointLightMax_ = 32;
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


	// 自キャラ
	std::unique_ptr<Player> player_;
	std::unique_ptr<Player2> player2_;

	std::unique_ptr<Model> modelPlayerBody_;
	std::unique_ptr<Model> modelPlayerHead_;
	std::unique_ptr<Model> modelPlayerL_arm_;
	std::unique_ptr<Model> modelPlayerR_arm_;

	std::unique_ptr<Model> modelPlayerBody2_;
	std::unique_ptr<Model> modelPlayerHead2_;
	std::unique_ptr<Model> modelPlayerL_arm2_;
	std::unique_ptr<Model> modelPlayerR_arm2_;

	//std::unique_ptr<Model> modelWepon_;

	std::array<std::unique_ptr<Flooar>, size_t(5)> flooars_;

	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<LockOn> lockOn_;


	std::unique_ptr<Particle> particle;

	std::unique_ptr<Model> modelBullet_;

	std::unique_ptr<PlayerAI> ai_;

	uint32_t backTextureHandle_;
	std::unique_ptr<Sprite> backSprite_;
	bool isEnd_;
	uint32_t endTextureHandle_[2];
	std::unique_ptr<Sprite> endSprite_;

	int32_t endCount_;
	bool isButtonDraw_;
	int32_t buttonCount_;
	
	uint32_t audioHandle_;
	int32_t shadeType_ = 0;
	uint32_t toonShadowTextureHandle_;

	std::unique_ptr<Sprite> fadeSprite_;
	bool isTransitionFade_;
	bool isStart_;
	float fadeAlpha_;
	bool isIngame_;

	Vector4 hsvFilter_;

	//メッシュシンク用テストオブジェクト
	std::unique_ptr<Terrain> terrain_;

	//skyboxcolor
	Vector4 skyColor_ = {0.45f,0.45f,0.45f,1.0f};
	Vector3 skyExpornent_ = {1.6f,1.6f,1.6f};
};
