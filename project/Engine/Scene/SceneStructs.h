#pragma once
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <list>
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
	EulerTransform deltaTransform;
	ColliderData collider;
	std::vector<GameObjectData> children;
};

//地形の頂点一個分のデータ
struct TerrainVerticesData {
	int32_t id;
	Vector3 position;
	Vector3 normal;
	Vector2 uv;
	int16_t specialFlag;
};

//地形生成用データ
struct TerrainData {
	std::string fileName = "";
	GameObjectData object;
	int32_t vertexNum_;
	std::list<TerrainVerticesData> verticesDatas;
};

//シーンのオブジェクト配置のでーた
struct SceneData {
	std::vector<GameObjectData> objects;
	//TerrainData terrainData;
};