#include "GameObject.h"

void GameObject::Initialize(const GameObjectData& data) {
	worldtransform_.Initialize();
	worldtransform_.translation_ = data.transform.translate;
	worldtransform_.rotation_ = data.transform.rotate;
	worldtransform_.scale_ = data.transform.scale;
	worldtransform_.UpdateMatrix();
	fileName = data.fileName;
	childlen_.clear();
}

void GameObject::Update() {
	worldtransform_.UpdateMatrix();
	if (!childlen_.empty()) {
		for (std::unique_ptr<GameObject>& child : childlen_) {
			child->SetParent(&worldtransform_);
			child->Update();
		}
	}
}

void GameObject::Draw(const ViewProjection& viewProjection, std::map<std::string, std::unique_ptr<Model>>& modelList) {
	auto it = modelList.find(fileName);
	if (it == modelList.end()) {
		return;
	}
	it->second->Draw(worldtransform_,viewProjection);
	if (!childlen_.empty()) {
		for (std::unique_ptr<GameObject>& child : childlen_) {
			child->Draw(viewProjection,modelList);
		}
	}
}

void GameObject::AppendChildlen(std::unique_ptr<GameObject> child) {
	childlen_.push_back(std::move(child));
}