#include "ModelComp.h"

void ModelComp::Update() {
	for (auto &model : modelTree_) {
		model->Update();
	}
}

void ModelComp::Draw(const Camera &vp) const {
	for (auto &model : modelTree_) {
		model->Draw(vp);
	}
}

void ModelComp::ImGuiWidget() {
	for (auto &item : modelTree_) {
		item->ImGuiWidget();
	}
}

void ModelComp::ModelBone::Init(std::unique_ptr<Model> model) {
	if (model) { model_ = std::move(model); }
}

void ModelComp::ModelBone::SetTransform(const Transform &srt) {
	transform_.scale = srt.scale;
	transform_.rotate = srt.rotate;
	transform_.translate = srt.translate;
}

ModelComp::ModelBone *const ModelComp::ModelBone::AddChild(std::unique_ptr<Model> model) {
	ModelBone *const modelBone = new ModelBone;
	modelBone->Init(std::move(model));

	AddChild(modelBone);
	return modelBone;
}

void ModelComp::ModelBone::AddChild(ModelBone *const child) {
	child->SetParent(this);
	children_.emplace_back(child);
}

void ModelComp::ModelBone::SetParent(ModelBone *const parent) {
	parent_ = parent;
	transform_.parent_ = &parent->transform_;
}

void ModelComp::ModelBone::Update() {

	model_->Update();

	transform_.CalcMatrix();

	model_->SetWorldMatrix(transform_.matWorld_);

	for (auto &child : children_) {
		child->Update();
	}
}

void ModelComp::ModelBone::Draw(const Camera &vp) const {
	if (model_) {
		this->model_->Draw(vp.GetViewProjection(), vp.GetPos());
	}
	for (auto &child : children_) {
		child->Draw(vp);
	}
}

bool ModelComp::ModelBone::ImGuiWidget() {
	bool result = false;
	/*result |= this->model_->ImGuiWidget();

	result |= this->transform_->ImGuiWidget(model_->GetObjFileName());*/

	//for (auto &item : this->children_) {
	//	result |= item->ImGuiWidget();
	//}
	return result;
}

ModelComp::ModelBone *const ModelComp::AddBone(const std::string &key, std::unique_ptr<Model> model, const Transform &srt) {
	if (modelKey_.count(key) != 0u) { return nullptr; }
	// ボーンの追加
	auto  newBone = std::make_unique<ModelBone>();
	// モデルを渡す
	newBone->Init(std::move(model));
	// GameObjectを親とする
	newBone->transform_.parent_ = &transform_;

	// transformのデータを渡す
	newBone->SetTransform(srt);
	
	// ボーンのキーを保存する
	modelKey_.insert({ key,newBone.get() });

	// ボーンのツリーに追加する
	modelTree_.push_back(std::move(newBone));

	// ボーンを返す
	return modelKey_.at(key);
}

ModelComp::ModelBone *const ModelComp::AddBone(const std::string &key, std::unique_ptr<Model> model, ModelBone *const parent, const Transform &srt) {
	if (modelKey_.count(key) != 0u) { return nullptr; }

	auto *const newBone = parent->AddChild(std::move(model));

	newBone->SetTransform(srt);

	modelKey_.insert({ key,newBone });

	return newBone;
}
