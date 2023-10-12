#pragma once
#include "BaseCharacter.h"
class Enemy : public BaseCharacter
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::vector<HierarchicalAnimation>& models) override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(const ViewProjection& ) override;
};
