#pragma once
#include "BasePlayerState.h"
class PlayerStateAttack : public BasePlayerState
{
public:
	~PlayerStateAttack() {};

	//初期化
	void Initialize() ;
	//実行
	void Update() ;

private:
	int frameCount_ = 0;
};
