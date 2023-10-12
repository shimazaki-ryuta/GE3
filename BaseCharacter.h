#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>


struct HierarchicalAnimation {
	Model* model_;
	WorldTransform worldTransform_;
};


class BaseCharacter 
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(const std::vector<HierarchicalAnimation>& models);
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw(const ViewProjection& viewProjection);

	inline WorldTransform* GetWorldTransform() { return &worldTransform_; };


protected:
	WorldTransform worldTransform_;
	std::vector<HierarchicalAnimation> models_;
};
