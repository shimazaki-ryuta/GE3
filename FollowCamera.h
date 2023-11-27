#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
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


	inline void SetTarget(WorldTransform* target) { target_ = target; };
	inline void SetLockOnTarget(WorldTransform* target) { lockOnTarget_ = target; };

	inline const ViewProjection& GetViewProjection() { return viewProjection_; };
	void Reset();
	void ApplyGlobalVariables();
	void Shake();
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
	float kShakeForce_=5;
};
