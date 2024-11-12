#pragma once
//#include <WinSock2.h>
//#include <WS2tcpip.h>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <cassert>
#include <json.hpp>
#include <filesystem>
#include <fstream>
#include <thread>
#include <memory>
#include "3D/Model.h"
#include "SceneStructs.h"
#include "GameObject/GameObject.h"
#include "Terrain/Terrain.h"
class SceneLoader
{
public:
	//jsonファイルから構造をロードする
	void LoadFile(const std::string fileName);
	//保持しているデータから必要なモデルを生成してリストに格納する
	void CreateModelList(std::map<std::string,std::unique_ptr<Model>>& list);
	void CreateObjects(std::vector<std::unique_ptr<GameObject>>& list);
	void CreateObject(std::unique_ptr<GameObject>& parent, GameObjectData& data);

	//terrain
	void CreateTerrain(std::unique_ptr<Terrain>& terrain);
	void ReadTerrainVertices(nlohmann::json& data);
	void ApplyTerrainVertices(std::unique_ptr<Terrain>& terrain);
	void ApplyTerrainTransform(std::unique_ptr<Terrain>& terrain);

	///debug
	void StartReceveJson();
	void EndReceveThread();
	void ReceveJsonData();

	void ApplyRecevedData(std::vector<std::unique_ptr<GameObject>>& list);
	void ScanChanged(std::unique_ptr<GameObject>& object, GameObjectData& datas, int32_t id);
	
	//変形したポリゴンのデータを作る
	//void CreateTerrainData();

private:
	//オブジェクト一個の解析
	void PraceObject(nlohmann::json& object,GameObjectData* parent=nullptr);

	bool isRecevedData_;
	bool isRecevedTerrain_;

	std::unique_ptr<SceneData> sceneData_;
	std::unique_ptr<TerrainData> terrainData_;

	std::thread receveJsonDataThread;
	SOCKET socket_;
};

