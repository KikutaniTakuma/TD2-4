#pragma once
#include "../GameObject.h"

#include <unordered_map>
#include <memory>

#include "Math/Transform.h"
#include "Drawers/Model/Model.h"

class ModelComp : public IComponent {
public:
	struct ModelBone {
		Transform transform_;
		std::unique_ptr<Model> model_;

		ModelBone *parent_ = nullptr;
		std::list<std::unique_ptr<ModelBone>> children_;

		void Init(std::unique_ptr<Model> model = nullptr);
		void SetTransform(const Transform &srt);

		ModelBone *const AddChild(std::unique_ptr<Model> model);
		void AddChild(ModelBone *const child);
		void SetParent(ModelBone *const parent);

		void Update();
		void Draw(const Camera &vp) const;

		bool ImGuiWidget();
	};

	//using ModelPair = std::pair<Transform, Model *>;
	//using ModelMap = std::unordered_map<std::string, ModelPair>;
	// 親のコンストラクタを使う
	using IComponent::IComponent;
	~ModelComp() override = default;

	//void SetModel(const ModelMap &model);

	/*ModelMap *const GetModel() {
		return &modelMap_;
	}

	ModelPair *const GetModel(const std::string &key) {
		auto it = modelMap_.find(key);
		if (it != modelMap_.end()) {
			return &it->second;
		}
		return nullptr;
	}*/


	void Update() override;
	void Draw(const Camera &vp)const override;

	void ImGuiWidget() override;

	ModelBone *const AddBone(const std::string &key, std::unique_ptr<Model> model, const Transform &srt = {});
	ModelBone *const AddBone(const std::string &key, std::unique_ptr<Model> model, ModelBone *const parent, const Transform &srt);

	ModelBone *const GetBone(const std::string &key) { return modelKey_.at(key); }

	const auto &GetModelTree() const { return modelTree_; }

private:
	std::list<std::unique_ptr<ModelBone>> modelTree_;
	std::unordered_map<std::string, ModelBone *> modelKey_;

};