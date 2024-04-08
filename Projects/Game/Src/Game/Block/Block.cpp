#include "Block.h"
#include "../SoLib/SoLib/SoLib_Traits.h"

void Block::Initialize(){
	pos_ = Vector3::kZero;

	model_ = std::make_unique<Model>("Resources/Cube.obj");

	collider_ = std::make_unique<Collider>();

	model_->scale = { vBlockScale->x,vBlockScale->y,vBlockScale->y };

	collider_->scale_ = model_->scale * 2.0f;

	isBreak_ = false;
}

void Block::Finalize(){

}

void Block::Update(){
	if (pos_.y < -10.0f) {
		isBreak_ = true;
	}
	if (pos_.y > 1.0f){
		pos_.y -= 0.2f;
	}
	

	model_->pos = pos_;
	
	model_->rotate.z = rotateZ_;

	model_->Update();

	collider_->collisionPos_ = model_->pos;
	

	collider_->UpdateCollision();
}

void Block::Draw(const Camera& camera){
	model_->Draw(camera.GetViewProjection(), camera.GetPos());
	collider_->DebugDraw(camera.GetViewProjection());
}

void Block::Debug(){
	ImGui::Begin("ボックス");
	ImGui::DragFloat3("ポジション", &pos_.x, 0.1f);
	ImGui::End();

	collider_->Debug("コライダー");
	model_->Debug("ボックスモデル");
}

void Block::Trade(){
	model_->pos = collider_->collisionPos_;
}
