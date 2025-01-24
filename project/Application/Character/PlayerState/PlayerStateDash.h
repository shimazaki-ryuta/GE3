#pragma once
#include "BasePlayerState.h"
class PlayerStateDash : public BasePlayerState
{
public:
	~PlayerStateDash() {};

	//初期化
	void Initialize() ;
	//実行
	void Update() ;

private:
	//ダッシュ中の速度
	float dashSpeed_ = 1.0f;
	//ダッシュのフレーム数
	int dashLength_ = 10;
	int frameCount_ = 0;
};
