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
#include "Model.h"
#include "SceneStructs.h"
#include "GameObject/GameObject.h"
class SceneLoader
{
public:
	//jsonファイルから構造をロードする
	void LoadFile(const std::string fileName);
	//保持しているデータから必要なモデルを生成してリストに格納する
	void CreateModelList(std::map<std::string,std::unique_ptr<Model>>& list);
	void CreateObjects(std::vector<std::unique_ptr<GameObject>>& list);
	void CreateObjects(std::unique_ptr<GameObject>& parent, GameObjectData& data);

	///debug
	void StartReceveJson();
	//closeportandThreadEnd
	//void EndReceveThread();
	void ReceveJsonData();
private:
	//オブジェクト一個の解析
	void PraceObject(nlohmann::json& object,GameObjectData* parent=nullptr);


	std::unique_ptr<SceneData> sceneData_;


	std::thread receveJsonDataThread;
	SOCKET socket_;
};

