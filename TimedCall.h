#pragma once
#include <functional>
#include <stdint.h>

class TimedCall {
public:
	TimedCall(std::function<void(void)> f, uint32_t time);
	void Update();
	bool IsFinished() { return finished; }

private:
	std::function<void(void)> f_;
	uint32_t time_;
	bool finished = false;
};
