#pragma once
#include <chrono>
class FixFPS
{
public:
	//FPS固定
	void Initialize();
	void Update();
private:
	std::chrono::steady_clock::time_point reference_;
};

