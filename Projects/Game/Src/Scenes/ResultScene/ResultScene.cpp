#include "ResultScene.h"

ResultScene::ResultScene():
BaseScene{ BaseScene::ID::Result }
{
}

void ResultScene::Initialize(){
	currentCamera_->pos = Vector2{ 500.f, 300.f };

	/*tex_.reset(new Texture2D("./Resources/Ball.png"));
	tex_->scale *= 30.0f;
	tex_->pos = Vector2{ 300.0f, 0.0f };*/
}

void ResultScene::Finalize(){

}

void ResultScene::Update(){
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();


	/*tex_->Debug("テスト用サークル");
	tex_->Update();*/
}

void ResultScene::Draw(){
	//tex_->Draw(currentCamera_->GetViewOthographics());
}

void ResultScene::Debug(){

}
