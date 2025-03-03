#pragma once
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"


//デバッグ用カメラ
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

	//Getter/Setter
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
