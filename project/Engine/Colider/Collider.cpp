#include "Collider.h"

Collider::Collider() {
	isCollision_ = true;
}

/*
void Collider::SetOnCollisionEvent(std::function<void(Collider&)> event_) {
	onCollisionEvent = event_;
}*/

void Collider::SetOnCollisionEvent(std::function<void()> event_) {
	onCollisionEvent = event_;
}

void Collider::OnCollision(){
	if (onCollisionEvent) {
		onCollisionEvent();
	}
	/*else if (onCollisionEventNULL) {
		onCollisionEventNULL();
	}*/
}

void Collider::SetSphere(Sphere& sphere) {
	colliderType_ = ColliderType::Shpere;
	collider_.sphere = sphere;
}
void Collider::SetAABB(AABB& AABB) {
	colliderType_ = ColliderType::AABB;
	collider_.aabb = AABB;
}
void Collider::SetOBB(OBB& OBB) {
	colliderType_ = ColliderType::OBB;
	collider_.obb = OBB;
}
