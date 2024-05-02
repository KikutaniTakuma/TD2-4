#include "Block.h"
#include "../SoLib/SoLib/SoLib_Traits.h"
#include "Drawers/DrawerManager.h"

void Block::Initialize(){
	pos_ = Vector3::kZero;

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");

	obb_ = std::make_unique<Obb>();

	transform_ = std::make_unique<Transform>();
	transform_->scale = { vBlockScale->x,vBlockScale->y,vBlockScale->y };

	obb_->scale_ = { transform_->scale.x,transform_->scale.y * 2.0f,transform_->scale.z * 2.0f };

	isBreak_ = false;
}

void Block::Finalize(){

}

void Block::Update(){
	
	if (pos_.y < -10.0f) {
		isBreak_ = true;
	}
	if (pos_.y > 1.0f){
		if (obb_->OnStay()){

		}
		else {
			pos_.y -= 0.4f;
		}
		
	}
	

	transform_->translate = pos_;
	
	// transform_->rotate.z = rotateZ_;

	transform_->CalcMatrix();

	obb_->center_ = transform_->translate;
	obb_->Update();
	

	
}

void Block::Draw(const Camera& camera){
	model_->Draw(transform_->matWorld_, camera.GetViewOthographics(), 0xffffffff, BlendType::kNormal);
}

void Block::Debug(){
	ImGui::Begin("ボックス");
	ImGui::DragFloat3("ポジション", &pos_.x, 0.1f);
	ImGui::End();

	obb_->Debug("コライダー");
	transform_->ImGuiWidget("ボックスモデル");
}

void Block::Trade(){
	transform_->translate = obb_->center_;
}
