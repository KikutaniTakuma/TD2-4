#include "TitleScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"

TitleScene::TitleScene():
	BaseScene{BaseScene::ID::Title}
{
}

void TitleScene::Initialize()
{
	camera_->pos.y = 2.85f;
	camera_->pos.z = -10.0f;
	camera_->rotate.x = 0.21f;

	itemManager_.reset(new ItemManager);
	itemManager_->Initialize();
}

void TitleScene::Finalize(){
	itemManager_->Finalize();
	
}

void TitleScene::Update()
{
	camera_->Debug("カメラ");
	camera_->Update(Vector3::kZero);

	itemManager_->Update();
	itemManager_->Debug();

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
}

void TitleScene::Draw()
{
	itemManager_->Draw(camera_.get());

	Lamb::screenout << "Model scene" << Lamb::endline
		<< "Press space to change ""Water and cloud scene""";
}

void TitleScene::Debug(){


}
