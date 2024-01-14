#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
class DebugCamera
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

	void SetUses(bool is){ isUses_ = is; };
	inline const ViewProjection& GetViewProjection() { return viewProjection_; };

	void SetRotate(const Vector3& rotate) { viewProjection_.rotation_ = rotate; };
	void SetPosition(const Vector3& position) { viewProjection_.translation_ = position; };
private:
	// ビュープロジェクション
	ViewProjection viewProjection_;

	WorldTransform* target_ = nullptr;

	bool isUses_;
};
