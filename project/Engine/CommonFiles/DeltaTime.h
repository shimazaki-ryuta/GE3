#pragma once
#include <intsafe.h>

//デルタタイム計算クラス

class DeltaTime
{
public:
	//インスタンス取得
	static DeltaTime* GetInstance();
	//ゲームループ開始
	static void GameLoopStart();
	//フレーム長計測開始
	static void FrameStart();
	//フレーム毎秒を返す
	static float GetFramePerSecond();
	//デルタタイム取得
	static float GetDeltaTime();

private:
	DeltaTime() = default;
	~DeltaTime() = default;
	DeltaTime(const DeltaTime&) = delete;
	DeltaTime operator=(const DeltaTime&) = delete;
	DWORD frameStart;
	DWORD preFrame;
	float deltaTime = 1.0f/60.0f;
};

