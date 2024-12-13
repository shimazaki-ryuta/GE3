#pragma once
#include <random>

//randam簡易利用用クラス

class RandomEngine
{
public:
	//インスタンス取得
	static RandomEngine* GetInstance() {
		static RandomEngine instance;
		return &instance;
	};
	//ランダムエンジン自体を取得
	std::mt19937& GetRandomEngine() { return randomEngine_; };

	//最小最大の範囲でランダムな値取得
	static float GetRandom(float min, float max) {
		std::uniform_real_distribution<float> distribution(min,max);
		return distribution(GetInstance()->GetRandomEngine());
	};
private:
	RandomEngine() {
		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());
		randomEngine_ = randomEngine;
	};
	~RandomEngine() = default;
	RandomEngine(const RandomEngine&) = delete;
	RandomEngine operator=(const RandomEngine&) = delete;
	
	std::mt19937 randomEngine_;
};

