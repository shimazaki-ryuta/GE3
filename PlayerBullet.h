#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
#include "Collider.h"
#include <memory>
class Enemy;

///<summary>
///自キャラの弾
///</summary>
class PlayerBullet : public Collider {
public:

	const float Radius = 1.0f;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>]
	/// <param name="positon">初期座標</param>
	/// <param name="velocity">速度</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity, std::weak_ptr<Enemy>);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	inline bool IsDead() const { return isDead_; };

	Vector3 GetWorldPosition() override;
	inline float GetRadius() override { return Radius; };
	// 衝突時に呼び出されるコールバック関数
	void OnCollision() override;

private:
	WorldTransform worldTransForm_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_;

	//寿命
	static const int32_t kLifeTime = 60 * 5;

	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;

	std::weak_ptr<Enemy> enemy_;
};
