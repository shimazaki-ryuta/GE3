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

	void Inirialize(ColliderData data);

	//OnCollisionがよばれたときに実行するイベントをセットする
	//void SetOnCollisionEvent(std::function<void(Collider&)>);
	void SetOnCollisionEvent(std::function<void()>);

	void OnCollision();

	//void SetType(const ColliderType&);
	void SetSphere(Sphere& );
	void SetAABB(AABB& );
	void SetOBB(OBB& );
	//virtual Vector3 GetWorldPosition()=0;
	//virtual float GetRadius() = 0;

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
