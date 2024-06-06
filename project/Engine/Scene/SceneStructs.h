#pragma once
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include "ModelStruct.h"

struct ColliderData {
	//std::string name="";
	std::string type = "";
	EulerTransform transform;
};

//オブジェクト一個のデータ
struct GameObjectData {
	std::string name = "";
	int32_t id;//同ステージ内の通し番号
	std::string type = "";
	std::string fileName = "";
	EulerTransform transform;
	ColliderData collider;
	std::vector<GameObjectData> children;
};

//シーンのオブジェクト配置のでーた
struct SceneData {
	std::vector<GameObjectData> objects;
};