#include "SelectScene.h"

SelectScene::SelectScene():
	BaseScene{ BaseScene::ID::StageSelect }
{

}

void SelectScene::Initialize(){
	currentCamera_->pos = Vector2{ 500.f, 300.f };

	tex_.reset(new Texture2D("./Resources/enemy_popEffect.png"));
	tex_->scale *= 30.0f;
	tex_->pos = Vector2{ 500.0f, 0.0f };
}

void SelectScene::Finalize(){

}

void SelectScene::Update(){
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();

	tex_->Debug("テスト用サークル");
	tex_->Update();
}

void SelectScene::Draw(){
	tex_->Draw(currentCamera_->GetViewOthographics());
}

void SelectScene::Debug(){

}

