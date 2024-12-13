#pragma once
#include <chrono>

//FPS固定

class FixFPS
{
public:
	//初期化
	void Initialize();
	//待機処理
	void Update();
private:
	std::chrono::steady_clock::time_point reference_;
};

