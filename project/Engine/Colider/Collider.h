#pragma once
#include <cstdint>
#include "CollisionConfig.h"
#include "Sphere.h"
#include "Vector3.h"
#include "Plane.h"
//#include "Triangle.h"
#include "AABB.h"
#include "OBB.h"
#include <functional>
#include <memory>
#include "Scene/SceneStructs.h"
#include "3D/WorldTransform.h"
struct Vector3;

//コールバック用関数を持ち呼び出しを行うコライダークラス

class Collider
{
public:

	enum class ColliderType
	{
		Shpere,
		AABB,
		OBB
	};

	union ColliderShape {
		Sphere sphere;
		AABB aabb;
		OBB obb;
		Plane plane;
	};

	struct Data
	{
		ColliderShape offset;
		ColliderShape inst;
	};

	Collider();

	//初期化
	void Inirialize(ColliderData data);

	//OnCollisionがよばれたときに実行するイベントをセットする
	void SetOnCollisionEvent(std::function<void()>);

	//衝突(Managerが呼び出す)
	void OnCollision();

	//形状セット
	void SetSphere();
	void SetAABB();
	void SetOBB();
	
	//Gettrer/Setter
	inline uint32_t GetCollisionAttribute() { return collisionAttribute_; };
	inline void SetCollisionAttribute(uint32_t collisionAttribute) {
		collisionAttribute_ = collisionAttribute;
	};
	inline uint32_t GetCollisionMask() { return collisionMask_; };
	inline void SetCollisionMask(uint32_t collisionMask) {
		collisionMask_ = collisionMask;
	};
	void SetIsDraw(bool is) { isDraw_ = is; };
	void SetIsCollision(bool is) { isCollision_ = is; };
	bool GetIsCollision() { return isCollision_; };
	ColliderShape GetColliderShape() { return collider_->inst; };
	auto GetType() {return colliderType_; };

	//保有オブジェクトのワールドトランスフォームを取得
	void ApplyWorldTransform(WorldTransform& parent);

private:
	uint32_t collisionAttribute_ = 0xffffffff;
	uint32_t collisionMask_ = 0xffffffff;

	ColliderType colliderType_;
	std::unique_ptr<Data> collider_;

	std::function<void()> onCollisionEvent;
	//std::function<void()> onCollisionEventNULL;

	bool isCollision_;
	bool isDraw_;
};
