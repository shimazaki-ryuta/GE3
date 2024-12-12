#pragma once
#include <functional>
#include <stdint.h>

//関数の次元呼び出しを行うクラス

class TimedCall {
public:
	//実行したい関数,実行フレーム(現在から何フレーム後か)
	TimedCall(std::function<void(void)> f, uint32_t time);
	//更新
	void Update();
	//終了検知
	bool IsFinished() { return finished; }

private:
	std::function<void(void)> f_;
	uint32_t time_;
	bool finished = false;
};
