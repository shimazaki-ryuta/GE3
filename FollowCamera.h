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

	inline const ViewProjection& GetViewProjection() { return viewProjection_; };


private:
	// ビュープロジェクション
	ViewProjection viewProjection_;

	WorldTransform* target_ = nullptr;

};
