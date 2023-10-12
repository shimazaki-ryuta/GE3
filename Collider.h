#pragma once
#include <cstdint>
#include "CollisionConfig.h"
struct Vector3;

class Collider
{
public:

	virtual void OnCollision()=0;
	virtual Vector3 GetWorldPosition()=0;
	virtual float GetRadius() = 0;

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
};
