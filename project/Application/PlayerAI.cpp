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
	data_.behavior = Player2::Behavior::kRoot;
	if (/*p2_->GetBehavior() == Player2::Behavior::kDash || */ coolTime_ <= 0) {
		data_.behavior = Player2::Behavior::kAttack;
	}
	if (p2_->GetBehavior() == Player2::Behavior::kAttack) {
		coolTime_ = 180;
	}
	if (avoid_) {
		data_.behavior = Player2::Behavior::kDash;
		float r = RandomEngine::GetRandom(0.0f,1.0f);
		if (r > 0.5f) {
			direction_ *= -1.0f;
			data_.behavior = Player2::Behavior::kJump;
		}
		avoid_ = false;
	}
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
	//data_.move = Normalize(move);

	/*if (!isFirst_) {
		fireAverage_ = AverageCalc();
		if (frameCount_ - lastFire_ > fireAverage_ && fireAverage_ != 0) {
			//lastFire_ -= frameCount_;
			//frameCount_ = 0;
			direction_ *= -1.0f;
			avoid_ = true;
		}
	}*/
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
	/*if (isFirst_) {
		isFirst_ = false;
		lastFire_ = 0;
	}
	else {
		fireInterval_.push_back(frameCount_-lastFire_);
		lastFire_ = frameCount_;
		//frameCount_ = 0;
	}*/
	float r = RandomEngine::GetRandom(0.0f, 1.0f);
	if (r > 0.1f && p2_->GetBehavior() == Player2::Behavior::kRoot && coolTime_ <= 120) {
		avoid_ = true;
	}
}