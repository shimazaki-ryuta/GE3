#pragma once
#include <list>
class Collider;

class CollisionManager {
public:
	static CollisionManager* GetInstance();
	void CheckAllCollisions();
	inline void PushCollider(Collider* collider) {colliders_.push_back(collider);};
	inline void ClearList() { colliders_.clear(); };
private:
	std::list<Collider*> colliders_;

};
