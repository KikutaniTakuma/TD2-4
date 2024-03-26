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
	slime_ = std::make_unique<Slime>();
	slime_->Init();
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
	camera_->Debug("カメラ");
	camera_->Update(Vector3::kZero);

	slime_->Update();
}

void TitleScene::Draw()
{
	slime_->Draw(*camera_);

	Lamb::screenout << "Slime test" << Lamb::endline;
}
