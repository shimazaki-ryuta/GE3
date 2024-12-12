#pragma once
#include "3D/Model.h"
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"
#include <vector>

//親子関係アニメーションさせたいオブジェクトに継承させるクラス

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

	//Getter/Setter
	inline WorldTransform* GetWorldTransform() { return &worldTransform_; };
	void SetShadowTexture(uint32_t handle);
	void SetOutLineData(float width,const Vector4& color);
protected:
	WorldTransform worldTransform_;
	std::vector<HierarchicalAnimation> models_;
};
