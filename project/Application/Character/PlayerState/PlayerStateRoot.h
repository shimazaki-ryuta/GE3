#pragma once
#include "BasePlayerState.h"
#include "Vector3.h"
class PlayerStateRoot : public BasePlayerState
{
public:
	~PlayerStateRoot() {};
	
	//初期化
	void Initialize() ;
	//実行
	void Update() ;

private:
	Vector3 kGravity = {0,-1.0f,0};
};
