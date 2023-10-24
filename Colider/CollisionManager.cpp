#include "CollisionManager.h"
#include "Collider.h"
#include "Vector3.h"
#include<cmath>

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
}

void CollisionManager::CheckAllCollisions()
{
#pragma region すべての当たり判定

	for (std::list<Collider*>::iterator iteA = colliders_.begin(); iteA != colliders_.end();
	     ++iteA) {
		Collider* colliderA = *iteA;
		std::list<Collider*>::iterator iteB = iteA;
		iteB++;
		for (; iteB != colliders_.end(); ++iteB) {
			Collider* collierB = *iteB;
			CheckCollisionPair(colliderA, collierB);
		}
	}

#pragma endregion
}