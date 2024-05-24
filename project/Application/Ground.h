#pragma once
#include "Model.h"
#include "WorldTransform.h"

#include <memory>
#include "../Engine/Animation.h"
#include "Skeleton.h"

/// <summary>
/// 地面
/// </summary>
class Ground 
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);
	void DrawOutLine(const ViewProjection& viewProjection);

	void SetPerspectiveTextureHandle(uint32_t handle) { model_->SetPerspectivTextureHandle(handle); };

private:
	WorldTransform worldTransform_;
	std::unique_ptr<Model> model_;

	std::unique_ptr<Animation> testAnimation_;
	std::unique_ptr<Skeleton> testSkeleton_;
	SkinCluster cluster_;
	float t = 1.0f;
	float growStrength_=0.0f;
};
