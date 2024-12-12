#pragma once
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"

//追従カメラ

class FollowCamera
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//グローバル変数適用
	void ApplyGlobalVariables();

	//リセット
	void Reset();

	//カメラシェイク
	void Shake();

	//Getter/Setter
	inline void SetTarget(WorldTransform* target) { target_ = target; };
	inline void SetLockOnTarget(WorldTransform* target) { lockOnTarget_ = target; };
	inline const ViewProjection& GetViewProjection() { return viewProjection_; };

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;

	WorldTransform* target_ = nullptr;
	WorldTransform* lockOnTarget_ = nullptr;
	Vector3 interTargert_;
	//補間の強さ
	float cameraDelay_;

	Matrix4x4 rotateMatrix_;
	Matrix4x4 rotateMatrix2_;
	Matrix4x4 rotateMatrix3_;
	bool isShake_;
	float shakeForce_;
	float kShakeForce_ = 5;
};
