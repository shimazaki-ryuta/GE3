#include "CollisionManager.h"
#include "Collider.h"
#include "Collision.h"
#include "Vector3.h"
#include<cmath>

CollisionManager* CollisionManager::GetInstance() {
	static CollisionManager instance;
	return &instance;
}

void CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) ||
	    !(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask())) {
		return;
	}
	/*
	Vector3 posA = colliderA->GetWorldPosition(), posB = colliderB->GetWorldPosition();
	float distance = float(
	    std::pow(posB.x - posA.x, 2) + std::pow(posB.y - posA.y, 2) + std::pow(posB.z - posA.z, 2));
	if (distance <= std::pow(colliderA->GetRadius() + colliderB->GetRadius(), 2)) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
	*/
	if (colliderA->GetType() == Collider::ColliderType::Shpere) {
		Sphere sphereA = colliderA->GetColliderShape().sphere;
		if (colliderB->GetType() == Collider::ColliderType::Shpere) {
			Sphere sphereB = colliderB->GetColliderShape().sphere;
			if (IsCollision(sphereA, sphereB)) {
				colliderA->OnCollision();
				colliderB->OnCollision();
			}
			return;
		}
		if (colliderB->GetType() == Collider::ColliderType::AABB) {
			AABB aabbB = colliderB->GetColliderShape().aabb;
			if (IsCollision(sphereA, aabbB)) {
				colliderA->OnCollision();
				colliderB->OnCollision();
			}
			return;
		}
		if (colliderB->GetType() == Collider::ColliderType::Shpere) {
			OBB obbB = colliderB->GetColliderShape().obb;
			if (IsCollision(sphereA, obbB)) {
				colliderA->OnCollision();
				colliderB->OnCollision();
			}
			return;
		}
	}
	if (colliderA->GetType() == Collider::ColliderType::OBB) {
		OBB obbA = colliderA->GetColliderShape().obb;
		if (colliderB->GetType() == Collider::ColliderType::Shpere) {
			Sphere sphereB = colliderB->GetColliderShape().sphere;
			if (IsCollision(obbA, sphereB)) {
				colliderA->OnCollision();
				colliderB->OnCollision();
			}
			return;
		}
		if (colliderB->GetType() == Collider::ColliderType::AABB) {
			/*AABB aabbB = colliderB->GetColliderShape().aabb;
			if (IsCollision(sphereA, aabbB)) {
				colliderA->OnCollision();
				colliderB->OnCollision();
			}*/
			return;
		}
		if (colliderB->GetType() == Collider::ColliderType::Shpere) {
			OBB obbB = colliderB->GetColliderShape().obb;
			if (IsCollision(obbA, obbB)) {
				colliderA->OnCollision();
				colliderB->OnCollision();
			}
			return;
		}
	}
	
	//if (IsCollision(sphereA, colliderB->GetColliderShape())) {}
}

void CollisionManager::CheckAllCollisions()
{
#pragma region すべての当たり判定

	for (std::list<Collider*>::iterator iteA = colliders_.begin(); iteA != colliders_.end();
	     ++iteA) {
		Collider* colliderA = *iteA;
		if (!colliderA->GetIsCollision()) {
			break;
		}
		std::list<Collider*>::iterator iteB = iteA;
		iteB++;
		for (; iteB != colliders_.end(); ++iteB) {
			Collider* colliderB = *iteB;
			if (!colliderB->GetIsCollision()) {
				break;
			}
			CheckCollisionPair(colliderA, colliderB);
		}
	}

#pragma endregion
}