#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include <d3d12.h>
#include <wrl.h>
#include "ModelStruct.h"

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

	//再生モード
	enum PlayType {
		LOOP,
		ONE
	};

	void Initialize();
	void LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	void Update();

	void SetTime(float t) { time = t; };

	//アニメーション適応行列を取得する
	Matrix4x4 GetAnimationMatrix(const std::string& nodename);

	std::shared_ptr<AnimationData> GetAnimationData() { return data_; };

	//読み込み済みのアニメーションデータのポインタをセットする
	void SetAnimationData(std::shared_ptr<AnimationData> data) { 
		data_ = data;
		isLoadingAnimation_ = true;
	};

	void SetPlayType(PlayType type) { playType_ = type; };

	void SetPlaySpeed(float t) { playSpeed_ = t; };

	// デバイス
	static ID3D12Device* sDevice;

	static void SetDevice(ID3D12Device* device);

	ID3D12Resource* GetMatrixResource() {return matrixResource_.Get(); };

private:
	std::shared_ptr<AnimationData> data_;
	bool isLoadingAnimation_=false;
	float time = 0.0f;
	float playSpeed_=1.0f;//再生速度
	PlayType playType_ = LOOP;

	//Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	//Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	Microsoft::WRL::ComPtr<ID3D12Resource> matrixResource_;
	Matrix4x4* matrixData_ = nullptr;
};

