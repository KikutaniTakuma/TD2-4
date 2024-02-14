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

	model_.reset(new Model{ "./Resources/Ball.obj" });

	model_->light.ptRange = 5.0f;
	model_->light.ptPos = model_->pos;
	model_->light.ptPos.y = 3.8f;
	model_->light.ptColor = Vector3::kIdentity * 15.0f;

	sphere_.reset(new Sphere);
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
	camera_->Debug("カメラ");
	camera_->Update(Vector3::kZero);

	model_->Debug("テスト用モデル");
	model_->Update();

	sphere_->Debug("Sphere");
	sphere_->Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
}

void TitleScene::Draw()
{
	model_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	sphere_->Draw(camera_->GetViewProjection(), std::numeric_limits<uint32_t>::max());

	Lamb::screenout << "Model scene" << Lamb::endline
		<< "Press space to change ""Water and cloud scene""";
}
