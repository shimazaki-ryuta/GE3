#include "PlayerAI.h"
#include "RandomEngine.h"
#include "Math/VectorFunction.h"
#include "Math/MatrixFunction.h"
void PlayerAI::Initialize() {
	isFirst_ = true;
	frameCount_ = 0;
	direction_ = {-1.0f,0.0f,0.0f};
	avoid_ = false;
	coolTime_ = 100;
}

void PlayerAI::Reset() {

}

void PlayerAI::Update() {
	//ビヘイビア設定
	data_.behavior = Enemy::Behavior::kRoot;
	if (coolTime_ <= 0) {
		data_.behavior = Enemy::Behavior::kAttack;
	}
	if (p2_->GetBehavior() == Enemy::Behavior::kAttack) {
		coolTime_ = 180;
	}
	//回避判定
	if (avoid_) {
		data_.behavior = Enemy::Behavior::kDash;
		float r = RandomEngine::GetRandom(0.0f,1.0f);
		if (r > 0.5f) {
			direction_ *= -1.0f;
			data_.behavior = Enemy::Behavior::kJump;
		}
		avoid_ = false;
	}
	//入力のかわり
	Vector3 move{0,0,0};

	Vector3 s = p2_->GetWorldTransform()->GetWorldPosition();
	s.y = 0;
	Vector3 e = p1_->GetWorldTransform()->GetWorldPosition();
	e.y = 0;
	float distance = Length(e - s);
	if (distance > 30.0f) {
		move.z = 1.0f;
	}
	if (distance < 20.0f) {
		move.z = -1.0f;
	}
	move += direction_;

	move = Transform(move, DirectionToDIrection(Normalize(Vector3{ 0.0f,0.0f,1.0f }), Normalize(e-s)));

	data_.move = move;

	frameCount_++;
	coolTime_--;
}

int PlayerAI::AverageCalc() {
	int count = 0;
	int sum = 0;
	std::list<int>::reverse_iterator ite = fireInterval_.rbegin();
	//ite--;
	for (int index = 0; index < 3;index++) {
		if (ite != fireInterval_.rend()) {
			sum += (*ite);
			count++;
			ite++;
		}
		else {
			break;
		}
	}
	if (count != 0) {
		sum /= count;
	}
	
	return sum;
}

void PlayerAI::Fire() {
	
	//待機状態だったら回避する
	float r = RandomEngine::GetRandom(0.0f, 1.0f);
	if (r > 0.1f && p2_->GetBehavior() == Enemy::Behavior::kRoot && coolTime_ <= 120) {
		avoid_ = true;
	}
}