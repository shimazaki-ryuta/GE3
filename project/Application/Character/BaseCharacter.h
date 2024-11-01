#pragma once
#include "3D/Model.h"
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"
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
	//void DrawOutLine(const ViewProjection& viewProjection);
	inline WorldTransform* GetWorldTransform() { return &worldTransform_; };

	void SetShadowTexture(uint32_t handle);
	void SetOutLineData(float width,const Vector4& color);
protected:
	WorldTransform worldTransform_;
	std::vector<HierarchicalAnimation> models_;
};
