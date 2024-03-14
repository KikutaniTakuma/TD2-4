#include "ItemManager.h"

void ItemManager::Initialize(){
	model_.reset(new Model{ "./Resources/Ball.obj" });
	model_->scale = { 0.3f,0.3f,0.3f };

	model_->light.ptRange = 5.0f;
	model_->light.ptPos = model_->pos;
	model_->light.ptPos.y = 3.8f;
	model_->light.ptColor = Vector3::kIdentity * 15.0f;

	model2_.reset(new Model{ "./Resources/Ball.obj" });

	model2_->pos = { 0.0f,-5.0f,0.0f };
	model2_->scale = { 0.3f,0.3f,0.3f };
	model2_->light.ptRange = 5.0f;
	model2_->light.ptPos = model2_->pos;
	model2_->light.ptPos.y = 3.8f;
	model2_->light.ptColor = Vector3::kIdentity * 15.0f;

	modelList_.push_back(std::move(model_));
	modelList_.push_back(std::move(model2_));

	sphere_.reset(new Sphere);
}

void ItemManager::Finalize(){
	modelList_.clear();
}

void ItemManager::Update(){
	
	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		it->get()->Update();
	}

	sphere_->Debug("Sphere");
	sphere_->Update();
}

void ItemManager::Draw(const Camera* camera){
	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		it->get()->Draw(camera->GetViewProjection(), camera->GetPos());
	}
	sphere_->Draw(camera->GetViewProjection(), std::numeric_limits<uint32_t>::max());
}

void ItemManager::Debug(){
	ImGui::Begin("アイテムエディター", nullptr, ImGuiWindowFlags_MenuBar);
	int i = 0;

	if (ImGui::BeginMenuBar()){
		if (ImGui::BeginMenu("オブジェクト追加")) {
			if (ImGui::TreeNode("アイテム追加")) {
				ImGui::DragFloat("大きさ", &sphere_->radius, 0.01f);
				//ImGui::DragFloat3("回転", &sphere_->worldRoate.x, 0.01f);
				ImGui::DragFloat3("座標", &sphere_->translation.x, 0.01f);

				if (ImGui::Button("アイテム追加")) {
					AddItem(sphere_->translation,Vector3(sphere_->radius,sphere_->radius, sphere_->radius));
				}
				ImGui::TreePop();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("アイテムリスト")) {
			for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
				if (ImGui::TreeNode(("栄養[" + std::to_string(0 + i) + "]").c_str())) {

					ImGui::DragFloat3("大きさ", &it->get()->scale.x, 0.01f);
					if (ImGui::IsItemActive()) {
						it->get()->color = 0xff0000ff;
						changeSize_ = true;
					}
					else {
						changeSize_ = false;
						it->get()->color = 0xffffffff;
					}
					ImGui::DragFloat3("座標", &it->get()->pos.x, 0.01f);
					if (changeSize_ == false) {
						if (ImGui::IsItemActive()) {
							it->get()->color = 0xff0000ff;
						}
						else {
							it->get()->color = 0xffffffff;
						}
					}

					ImGui::TreePop();
				}
				i++;
			}			

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	/*for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		it->get()->Debug(("テスト用モデル" + std::to_string(1 + i)).c_str());
		it->get()->Update();
		i++;
	}*/
	ImGui::End();
}

void ItemManager::AddItem(const Vector3& pos, const Vector3& scale){
	auto& model = modelList_.emplace_back(std::make_unique<Model>("./Resources/Ball.obj"));
	model->pos = pos;
	model->scale = scale;
}
