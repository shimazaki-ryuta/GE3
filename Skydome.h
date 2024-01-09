#pragma once
#include "Model.h"
#include "WorldTransform.h"

#include <memory>

/// <summary>
/// 天球
/// </summary>
class Skydome {
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


private:
	WorldTransform worldTransform_;
	std::unique_ptr <Model> model_;
	uint32_t textureHandle_;
};
