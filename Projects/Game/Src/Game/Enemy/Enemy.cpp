#include "Enemy.h"

Enemy::~Enemy(){

}

void Enemy::Initialize(){
	pos_ = Vector3::kZero;

	model_ = std::make_unique<Model>("Resources/Cube.obj");

	obb_ = std::make_unique<Obb>();

	model_->scale = Vector3::kIdentity * 0.2f;

	obb_->scale_ = { model_->scale.x,model_->scale.y * 2.0f,model_->scale.z * 2.0f };

	isDead_ = false;
}

void Enemy::Update(){
	model_->pos = pos_;

	model_->Update();

	obb_->center_ = model_->pos;
	obb_->Update();
}

void Enemy::Draw(const Camera& camera){
	model_->Draw(camera.GetViewProjection(), camera.GetPos());

}

void Enemy::Debug(const size_t name){
	//ImGui::Begin("ボックス");
	ImGui::DragFloat3(("ポジション" + std::to_string(name)).c_str(), &pos_.x, 0.1f);
	//ImGui::End();

	//obb_->Debug("コライダー");
	//model_->Debug("ボックスモデル");
}
