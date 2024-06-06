#include "SceneLoader.h"

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

	sceneData_.reset(new SceneData);

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
		if (!parent) {
			sceneData_->objects.emplace_back(GameObjectData{});
			objectData = &sceneData_->objects.back();
		}
		else {
			parent->children.emplace_back(GameObjectData{});
			objectData = &parent->children.back();
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

		//collider
		if (object.contains("collider")) {
			nlohmann::json& collider = object["collider"];
			objectData->collider.type = collider["type"];
			if (objectData->collider.type.compare("BOX") == 0) {
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
		instance.reset(new GameObject);
		instance->Initialize(object);
		if (!object.children.empty()) {
			for (GameObjectData& child : object.children) {

				CreateObjects(instance,child);
			}
		}
		list.push_back(std::move(instance));
	}
}

void SceneLoader::CreateObjects(std::unique_ptr<GameObject>& parent, GameObjectData& data) {
	std::unique_ptr<GameObject> instance;
	instance.reset(new GameObject);
	instance->Initialize(data);
	if (!data.children.empty()) {
		for (GameObjectData& child : data.children) {

			CreateObjects(instance, child);
		}
	}
	parent->AppendChildlen(std::move(instance));
}