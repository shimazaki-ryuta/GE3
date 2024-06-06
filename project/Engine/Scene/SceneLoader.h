#pragma once
#include <variant>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include "ModelStruct.h"
#include <cassert>
#include <json.hpp>
#include <filesystem>
#include <fstream>
#include "Model.h"
struct ColliderData {
	//std::string name="";
	std::string type="";
	EulerTransform transform;
};

//オブジェクト一個のデータ
struct GameObjectData {
	std::string name="";
	int32_t id;//同ステージ内の通し番号
	std::string type="";
	std::string fileName="";
	EulerTransform transform;
	ColliderData collider;
	std::vector<GameObjectData> children;
};

//シーンのオブジェクト配置のでーた
struct SceneData {
	std::vector<GameObjectData> objects;
};

class SceneLoader
{
public:
	//jsonファイルから構造をロードする
	void LoadFile(const std::string fileName);
	//保持しているデータから必要なモデルを生成してリストに格納する
	void CreateModelList(std::map<std::string,std::unique_ptr<Model>>& list);
private:
	//オブジェクト一個の解析
	void PraceObject(nlohmann::json& object,GameObjectData* parent=nullptr);


	std::unique_ptr<SceneData> sceneData;

};

