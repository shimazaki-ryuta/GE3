#pragma once
#include "BasePlayerState.h"
#include "Vector3.h"
class PlayerStateJump : public BasePlayerState
{
public:
	~PlayerStateJump() {};

	//初期化
	void Initialize() ; 
	//実行
	void Update() ;

private:
	Vector3 kJumpVelocity = {0,0.3f,0};
};
