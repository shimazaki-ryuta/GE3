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

//シーン読み込みクラス

class SceneLoader
{
public:
	//jsonファイルから構造をロードする
	void LoadFile(const std::string fileName);
	//保持しているデータから必要なモデルを生成してリストに格納する
	void CreateModelList(std::map<std::string,std::unique_ptr<Model>>& list);
	//データからRootオブジェクトを生成する
	void CreateObjects(std::vector<std::unique_ptr<GameObject>>& list);
	//子オブジェクトを生成する
	void CreateObject(std::unique_ptr<GameObject>& parent, GameObjectData& data);

	//MeshSync
	//オブジェクトを生成する
	void CreateTerrain(std::unique_ptr<Terrain>& terrain);
	//受信Jsonから頂点情報を読む
	void ReadTerrainVertices(nlohmann::json& data);
	//頂点情報をモデルに適用する
	void ApplyTerrainVertices(std::unique_ptr<Terrain>& terrain);
	//トランスフォーム情報をモデルに適用する
	void ApplyTerrainTransform(std::unique_ptr<Terrain>& terrain);
	//jsonファイルから頂点情報を読む
	void ReadTerrainVerticesFromFile(std::unique_ptr<Terrain>& terrain,const std::string fileName);

	///debug

	//json受信用サブスレッドを開始する
	void StartReceveJson();
	//サブスレッドを開放する
	void EndReceveThread();
	//json受信処理
	void ReceveJsonData();

	//受信データ(Scene)を適用する
	void ApplyRecevedData(std::vector<std::unique_ptr<GameObject>>& list);
	//シーンの変更チェック
	void ScanChanged(std::unique_ptr<GameObject>& object, GameObjectData& datas);
	
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

