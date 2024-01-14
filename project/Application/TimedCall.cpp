#include "TimedCall.h"

TimedCall::TimedCall(std::function<void(void)> f, uint32_t time) : f_(f), time_(time)
{
}

void TimedCall::Update()
{
	if (finished) {
		return;
	}
	time_--;
	if (time_ <= 0) {
		finished = true;
		f_();
	}
}