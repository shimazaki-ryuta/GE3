#include "SceneLoader.h"
#include "ConvertString.h"
#include <functional>
#pragma comment(lib, "ws2_32.lib")//socket関係のライブラリ
#pragma warning(disable:4996)//4996エラーを無視する

void SceneLoader::LoadFile(const std::string fileName) {
	const std::string fullpath = "Resources/LevelData/" + fileName + ".json";

	std::ifstream file;

	file.open(fullpath);
	if (file.fail()) {
		assert(0);
	}

	nlohmann::json deserialized;

	file >> deserialized;

	//レベルデータが正しいかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	std::string name = deserialized["name"].get<std::string>();
	assert(name.compare("scene") == 0);

	sceneData_ = std::make_unique<SceneData>();
	terrainData_ = std::make_unique<TerrainData>();
	//全オブジェクト操作
	for (nlohmann::json& object : deserialized["objects"]) {
		PraceObject(object);
		
	}
}

void SceneLoader::PraceObject(nlohmann::json& object, GameObjectData* parent) {
	std::string type = object["type"].get<std::string>();
	if (type.compare("MESH") == 0) {
		static GameObjectData preObjectData;//仮参照用ローカル変数
		GameObjectData* objectData = nullptr;
		//要素追加

		//terrainの方に格納
		if (object["name"] == "field") {
			objectData = &terrainData_->object;
		}
		else {
			if (!parent) {
				sceneData_->objects.emplace_back(GameObjectData{});
				objectData = &sceneData_->objects.back();
			}
			else {
				parent->children.emplace_back(GameObjectData{});
				objectData = &parent->children.back();
			}
		}
		//タイプ指定
		if (object.contains("type")) {
			objectData->type = object["type"];
		}

		//モデルファイル読み込み
		if (object.contains("file_name")) {
			objectData->fileName = object["file_name"];
		}
		objectData->id = (int32_t)object["id"];

		//トランスフォーム読み込み
		nlohmann::json& transform = object["transform"];
		//t
		objectData->transform.translate.x = (float)transform["translation"][0];
		objectData->transform.translate.y = (float)transform["translation"][2];
		objectData->transform.translate.z = (float)transform["translation"][1];
		//r
		objectData->transform.rotate.x = -(float)transform["rotation"][0];
		objectData->transform.rotate.y = -(float)transform["rotation"][2];
		objectData->transform.rotate.z = -(float)transform["rotation"][1];
		//s
		objectData->transform.scale.x = (float)transform["scaling"][0];
		objectData->transform.scale.y = (float)transform["scaling"][2];
		objectData->transform.scale.z = (float)transform["scaling"][1];

		//deltaTransform
		objectData->deltaTransform.translate = {0,0,0};
		objectData->deltaTransform.rotate = { 0,0,0 };
		objectData->deltaTransform.scale = { 1.0f,1.0f,1.0f };
		if (object.contains("delta_transform")) {
			nlohmann::json& dtransform = object["delta_transform"];
			//t
			objectData->deltaTransform.translate.x = (float)dtransform["translation"][0];
			objectData->deltaTransform.translate.y = (float)dtransform["translation"][2];
			objectData->deltaTransform.translate.z = (float)dtransform["translation"][1];
			//r
			objectData->deltaTransform.rotate.x = -(float)dtransform["rotation"][0];
			objectData->deltaTransform.rotate.y = -(float)dtransform["rotation"][2];
			objectData->deltaTransform.rotate.z = -(float)dtransform["rotation"][1];
			//s
			objectData->deltaTransform.scale.x = (float)dtransform["scaling"][0];
			objectData->deltaTransform.scale.y = (float)dtransform["scaling"][2];
			objectData->deltaTransform.scale.z = (float)dtransform["scaling"][1];
		}

		//collider
		if (object.contains("collider")) {
			nlohmann::json& collider = object["collider"];
			objectData->collider.type = collider["type"];
			if (objectData->collider.type.compare("BOX") == 0 || objectData->collider.type.compare("SPHERE") == 0) {
				objectData->collider.transform.translate.x = (float)collider["center"][0];
				objectData->collider.transform.translate.y = (float)collider["center"][2];
				objectData->collider.transform.translate.z = (float)collider["center"][1];
				
				objectData->collider.transform.rotate = {0,0,0};

				objectData->collider.transform.scale.x = (float)collider["size"][0];
				objectData->collider.transform.scale.y = (float)collider["size"][2];
				objectData->collider.transform.scale.z = (float)collider["size"][1];
			}
		}
		//再帰走査
		if (object.contains("children")) {
			for (nlohmann::json& children : object["children"]) {
				PraceObject(children, objectData);
			}
		}
	}
}

void SceneLoader::CreateModelList(std::map<std::string, std::unique_ptr<Model>>& list) {
	for (GameObjectData& object: sceneData_->objects) {
		Model* model = nullptr;
		if (!object.fileName.empty()) {

			// 読み込み済みモデルだったら何もせずコンティニュー
			auto it = std::find_if(list.begin(), list.end(), [&](const auto& datas) {
				return object.fileName == datas.first;
				});
			if (it != list.end()) {
				continue;
			}
			model = new Model;
			model->Create("Resources/Models", object.fileName);
			list.emplace(object.fileName, model);
		}
		if (!object.children.empty()) {
			for (GameObjectData& child : object.children) {
				if (child.fileName.empty()) {
					continue;
				}
				// 読み込み済みモデルだったら何もせずコンティニュー
				auto it = std::find_if(list.begin(), list.end(), [&](const auto& datas) {
					return child.fileName == datas.first;
					});
				if (it != list.end()) {
					continue;
				}
				model = new Model;
				model->Create("Resources/Models", child.fileName);
				list.emplace(child.fileName, model);
			}
		}
	}
}

void SceneLoader::CreateObjects(std::vector<std::unique_ptr<GameObject>>& list) {
	for (GameObjectData& object : sceneData_->objects) {
		std::unique_ptr<GameObject> instance;
		instance = std::make_unique<GameObject>();
		instance->Initialize(object);
		if (!object.children.empty()) {
			for (GameObjectData& child : object.children) {

				CreateObject(instance,child);
			}
		}
		list.push_back(std::move(instance));
	}
}

void SceneLoader::CreateObject(std::unique_ptr<GameObject>& parent, GameObjectData& data) {
	std::unique_ptr<GameObject> instance;
	instance = std::make_unique<GameObject>();
	instance->Initialize(data);
	if (!data.children.empty()) {
		for (GameObjectData& child : data.children) {

			CreateObject(instance, child);
		}
	}
	parent->AppendChildlen(std::move(instance));
}

void SceneLoader::CreateTerrain(std::unique_ptr<MeshSyncObject>& terrain) {
	terrain = std::make_unique<MeshSyncObject>();
	terrain->Initialize(terrainData_->object);
}

void SceneLoader::ApplyTerrainTransform(std::unique_ptr<MeshSyncObject>& terrain) {
	if (!isRecevedData_) {
		return;
	}
	terrain->SetTransformData(*(terrainData_.get()));
}

//debug
void SceneLoader::StartReceveJson() {

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	isRecevedData_ = false;
	isRecevedTerrain_ = false;
	//portSetUp
	socket_ = socket(AF_INET,//ipv4
		SOCK_DGRAM,//udp tcpの場合はSOCK_STREAM
		0);
	//ポート番号と送信先IPアドレスの設定ローカルだから127.0.0.1
	sockaddr_in address_;
	address_.sin_family = AF_INET;
	address_.sin_port = htons(50001);
	address_.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//portOpen
	bind(socket_, (struct sockaddr*)&address_, sizeof(address_));

	//receveStart
	receveJsonDataThread = std::thread(std::bind(&SceneLoader::ReceveJsonData, this));
	//ReceveJsonData();
}

void SceneLoader::EndReceveThread() {
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(50001);
	address.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	char buf[16] = "{""end"" : 0 }";
	sendto(socket_, buf, 16, 0, (sockaddr*)&address, sizeof(address));
	receveJsonDataThread.join();
}

void SceneLoader::ReceveJsonData() {
	static bool isEnd = false;
	static int sockaddr_in_size = sizeof(struct sockaddr_in);
	sockaddr_in from;
	const static uint32_t buffSize = 32768;
	static char rBuff[buffSize];
	std::string sData;
	while (!isEnd) {
		//受信
		recvfrom(socket_, rBuff, buffSize , 0, (sockaddr*)&from, &sockaddr_in_size);

		//end
		if (strstr(rBuff, "end")) {
			break;
		}

		//受信データをコピー
		//ApplyVertexFromBinary(rBuff);
		//ApplyMaterial(rBuff);
		AppyTextureName(rBuff);
		
	}
	closesocket(socket_);
	isEnd = true;
	WSACleanup();
}

void SceneLoader::ApplyRecevedData(std::vector<std::unique_ptr<GameObject>>& list) {
	if (!isRecevedData_) {
		return;
	}
	
	//ルート走査
	size_t index = 0;
	//削除チェック
	
	//変更チェック
	index = 0;
	for (std::unique_ptr<GameObject> &object : list) {
		if (index < sceneData_->objects.size()) {
			ScanChanged(object, sceneData_->objects[index]);
		}
		index++;
	}


	//追加チェック
	for (index; index < sceneData_->objects.size(); index++) {
		//追加処理
		std::unique_ptr<GameObject> instance;
		instance = std::make_unique<GameObject>();
		instance->Initialize(sceneData_->objects[index]);
		if (!sceneData_->objects[index].children.empty()) {
			for (GameObjectData& child : sceneData_->objects[index].children) {

				CreateObject(instance, child);
			}
		}
		list.push_back(std::move(instance));
	}
	isRecevedData_ = false;
}

void SceneLoader::ScanChanged(std::unique_ptr<GameObject>& object,GameObjectData& datas) {
	//本体更新
	object->SetParameter(datas);

	size_t index = 0;

	//子要素変更チェック
	index = 0;
	for (std::unique_ptr<GameObject>& child : *object->GetChildlen()) {
		ScanChanged(child, datas.children[index]);
		index++;
	}

	//子要素追加
	for (index; index < datas.children.size(); index++) {
		CreateObject(object, datas.children[index]);
	}
}

void SceneLoader::ApplyVertexFromBinary(char* binData) {
	
	//通信ヘッダー
	MeshSyncHeader header;

	//頂点数取得
	memcpy(&header, binData, sizeof(MeshSyncHeader));

	if (bool(header.isLastData) || verticesNum_ < header.verticesNum) {
		verticesNum_ = header.verticesNum;
	}

	//ヘッダー分のオフセット
	const static size_t headerSize = sizeof(MeshSyncHeader);
	memcpy(vData_ + header.verticesOffset, binData + headerSize, sizeof(VertexData) * (header.verticesNum - header.verticesOffset));
}

void SceneLoader::ApplyMaterial(char* binData) {
	struct RecvMaterial
	{
		Vector4 diffuseColor;
		float specularPow;
		float metaric;
	};
	RecvMaterial m;
	memcpy(&m,binData,sizeof(RecvMaterial));
	mData_->color = m.diffuseColor;
	mData_->shininess = m.specularPow;
	mData_->environmentCoefficient = m.metaric;
}

void SceneLoader::AppyTextureName(char* binData) {
	*textureName_ = binData;
}