#include "Collider.h"

Collider::Collider() {
	isCollision_ = true;
}

void Collider::Inirialize(ColliderData data) {
	collider_.reset(new Data);
	if (data.type == "SPHERE") {
		collider_->offset.sphere.center = data.transform.translate;
		collider_->offset.sphere.radius = data.transform.scale.x * 0.5f;
		collider_->inst = collider_->offset;
		colliderType_ = ColliderType::Shpere;
	}
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
	//collider_.sphere = sphere;
}
void Collider::SetAABB(AABB& AABB) {
	colliderType_ = ColliderType::AABB;
	//collider_.aabb = AABB;
}
void Collider::SetOBB(OBB& OBB) {
	colliderType_ = ColliderType::OBB;
	//collider_.obb = OBB;
}

void Collider::ApplyWorldTransform(WorldTransform& parent){
	switch (colliderType_)
	{
	case Collider::ColliderType::Shpere:
		collider_->inst.sphere.center = Transform(collider_->offset.sphere.center,parent.matWorld_);
		break;
	case Collider::ColliderType::AABB:
		break;
	case Collider::ColliderType::OBB:
		break;
	default:
		break;
	}
}