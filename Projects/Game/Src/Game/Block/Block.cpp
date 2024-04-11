#include "Block.h"
#include "../SoLib/SoLib/SoLib_Traits.h"

void Block::Initialize(){
	pos_ = Vector3::kZero;

	model_ = std::make_unique<Model>("Resources/Cube.obj");

	obb_ = std::make_unique<Obb>();

	model_->scale = { vBlockScale->x,vBlockScale->y,vBlockScale->y };

	obb_->scale_ = { model_->scale.x,model_->scale.y * 2.0f,model_->scale.z * 2.0f };

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
	

	model_->pos = pos_;
	
	model_->rotate.z = rotateZ_;

	model_->Update();

	obb_->center_ = model_->pos;
	obb_->Update();
	

	
}

void Block::Draw(const Camera& camera){
	model_->Draw(camera.GetViewProjection(), camera.GetPos());
}

void Block::Debug(){
	ImGui::Begin("ボックス");
	ImGui::DragFloat3("ポジション", &pos_.x, 0.1f);
	ImGui::End();

	obb_->Debug("コライダー");
	model_->Debug("ボックスモデル");
}

void Block::Trade(){
	model_->pos = obb_->center_;
}
