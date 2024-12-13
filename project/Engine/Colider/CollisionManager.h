#pragma once
#include <list>
class Collider;

//コライダーの所有権を持ち、コライダーの管理を行うクラス

class CollisionManager {
public:
	//インスタンス取得
	static CollisionManager* GetInstance();
	//登録された全当たり判定を走査する
	void CheckAllCollisions();
	//コライダーの実体を登録する
	inline void PushCollider(Collider* collider) {colliders_.push_back(collider);};
	//コライダーリストクリア
	inline void ClearList() { colliders_.clear(); };
private:
	std::list<Collider*> colliders_;

};
