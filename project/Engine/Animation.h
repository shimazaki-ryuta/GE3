#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
template<typename tValue>
struct Keyframe {
	float time;
	tValue value;
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

struct NodeAnimation {
	std::vector<KeyframeVector3> translate;
	std::vector<KeyframeQuaternion> rotate;
	std::vector<KeyframeVector3> scale;
};

struct AnimationData {
	float duration;
	std::map<std::string, NodeAnimation> nodeAnimations;
};

class Animation
{
public:
	void Initialize() {
		isLoadingAnimation_ = false;
		time = 0.0f;
	};
	void LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	void Update() {
		if (isLoadingAnimation_) {
			time += 1.0f / 60.0f;
			time = std::fmod(time, data->duration);
		}
	}

	//アニメーション適応行列を取得する
	//Matrix4x4 GetAnimationMatrix(const std::string& nodename);

private:
	std::unique_ptr<AnimationData> data;
	bool isLoadingAnimation_=false;
	float time = 0.0f;
	

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
};

