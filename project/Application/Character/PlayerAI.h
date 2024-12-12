#pragma once
#include "Player.h"
#include "Player2.h"
class PlayerAI
{
public:
	//初期化
	void Initialize();
	//更新
	void Update();

	//プレイヤーの射撃の平均間隔
	int AverageCalc();

	//プレイヤーの射撃に対して
	void Fire();
	//リセット
	void Reset();

	//Getter/Setter
	Player2::ReceveData& GetData() { return data_; };
	void SetPlayer1(Player* p) { p1_ = p; };
	void SetPlayer2(Player2* p) { p2_ = p; };
private:
	Player2::ReceveData data_;
	Vector3 direction_;
	int fireAverage_;
	int lastFire_;
	int frameCount_;
	int coolTime_;
	bool avoid_;
	std::list<int> fireInterval_;
	bool isFirst_;
	Player* p1_;
	Player2* p2_;
};

