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

	void OnCollision();
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

private:
	uint32_t collisionAttribute_ = 0xffffffff;
	uint32_t collisionMask_ = 0xffffffff;

	ColliderShape collider_;
};
