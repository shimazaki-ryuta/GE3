#include "GameObject.h"
#include "MatrixFunction.h"
void GameObject::Initialize(const GameObjectData& data) {
	worldtransform_.Initialize();
	worldtransform_.translation_ = data.transform.translate;
	worldtransform_.rotation_ = data.transform.rotate;
	worldtransform_.scale_ = data.transform.scale;
	worldtransform_.UpdateMatrix();
	deltaTransform_ = data.deltaTransform;
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale,deltaTransform_.rotate,deltaTransform_.translate);
	fileName = data.fileName;
	childlen_.clear();

	material_.reset(new Material);
	material_->Initialize();
	
	//collider
	if (!data.collider.type.empty()) {
		collider_.reset(new Collider);
		collider_->Inirialize(data.collider);
	}
}

void GameObject::SetParameter(const GameObjectData& data) {
	worldtransform_.translation_ = data.transform.translate;
	worldtransform_.rotation_ = data.transform.rotate;
	worldtransform_.scale_ = data.transform.scale;
	worldtransform_.UpdateMatrix();
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);
	fileName = data.fileName;
	if (!data.collider.type.empty()) {
		collider_.reset(new Collider);
		collider_->Inirialize(data.collider);
	}
}

void GameObject::Update() {
	worldtransform_.UpdateMatrix();
	worldtransform_.matWorld_ *= MakeAffineMatrix(deltaTransform_.scale, deltaTransform_.rotate, deltaTransform_.translate);
	if (!childlen_.empty()) {
		for (std::unique_ptr<GameObject>& child : childlen_) {
			child->SetParent(&worldtransform_);
			child->Update();
		}
	}
	if (collider_.get()) {
		collider_->ApplyWorldTransform(worldtransform_);
	}
}

void GameObject::Draw(const ViewProjection& viewProjection, std::map<std::string, std::unique_ptr<Model>>& modelList) {
	auto it = modelList.find(fileName);
	if (it == modelList.end()) {
		return;
	}
	it->second->SetMaterial(material_.get());
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

void GameObject::AppendColliderList(std::list<Collider*>& list) {
	if (collider_.get()) {
		if (collider_->GetType() == Collider::ColliderType::Shpere) {
			list.push_back(collider_.get());
		}
	}
	if (!childlen_.empty()) {
		for (std::unique_ptr<GameObject>& child : childlen_) {
			child->AppendColliderList(list);
		}
	}
}