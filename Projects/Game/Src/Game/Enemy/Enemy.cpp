#include "Enemy.h"
#include "Drawers/DrawerManager.h"

Enemy::~Enemy(){

}

void Enemy::Initialize(){
	transform.translate = Vector3::kZero;

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");

	model_ = drawerManager->GetModel("Resources/Cube.obj");

	obb_ = std::make_unique<Obb>();

	transform.scale = Vector3::kIdentity * 0.2f;

	obb_->scale_ = { transform.scale.x,transform.scale.y * 2.0f,transform.scale.z * 2.0f };

	isDead_ = false;
}

void Enemy::Update(){
	transform.CalcMatrix();
	obb_->center_ = transform.translate;
	obb_->Update();
}

void Enemy::Draw(const Camera& camera){
	model_->Draw(transform.matWorld_, camera.GetViewProjection(), 0xffffffff, BlendType::kNormal);

}

void Enemy::Debug(const size_t name){
	//ImGui::Begin("ボックス");
	ImGui::DragFloat3(("ポジション" + std::to_string(name)).c_str(), transform.translate.data(), 0.1f);
	//ImGui::End();

	//obb_->Debug("コライダー");
	//model_->Debug("ボックスモデル");
}
