#include "TitleScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include "Game/CollisionManager/Capsule/Capsule.h"

TitleScene::TitleScene():
	BaseScene{BaseScene::ID::Title}
{
}

void TitleScene::Initialize(){
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();	

	currentCamera_->pos = { 0.f, 0.f ,-1.0f };
	shakePower_ = { 5.0f,10.0f };

	titleBGM_ = audioManager_->Load("./Resources/Sounds/BGM/title.mp3");
	beginGame_ = audioManager_->Load("./Resources/Sounds/SE/choice.mp3");
	titleBGM_->Start(0.01f, true);

	titleDirection_ = TitleDirection::GetInstance();

	titleDirection_->Initialize();

}

void TitleScene::Finalize(){
	titleBGM_->Stop();

}

void TitleScene::Update()
{
	currentCamera_->Debug("カメラ");
	currentCamera_->Shake(1.0f);
	currentCamera_->Update();

	auto* const key = input_->GetKey();


	if (key->Pushed(DIK_SPACE) && !titleDirection_->GetIsFirstFade()) {
		beginGame_->Start(0.1f, false);
		sceneManager_->SceneChange(BaseScene::ID::StageSelect);
	}

	titleDirection_->Update(input_);
	if (titleDirection_->GetIsActiveExit()){
		currentCamera_->BeginShake(shakePower_);
	}
	
	Debug();

}

void TitleScene::Draw(){

	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());

	titleDirection_->Draw(currentCamera_->GetViewOthographics());

#ifdef _DEBUG

	UIEditor::GetInstance()->PutDraw(currentTexCamera_->GetViewOthographics());
#endif // _DEBUG
}

void TitleScene::TimerMove(){

}

void TitleScene::SetControlPoint(){
	////第一ポイントの位置によって+-を変更
	////画面の左側なら
	//if (ControlPoints_[0].x < ControlPoints_[3].x) {
	//	ControlPoints_[1] = ControlPoints_[0] + Vector2(-secondPointPos_.x, secondPointPos_.y);
	//}
	//else {
	//	ControlPoints_[1] = ControlPoints_[0] + Vector2(secondPointPos_.x, secondPointPos_.y);
	//}

	//if (ControlPoints_[0].x < ControlPoints_[3].x) {
	//	ControlPoints_[2] = ((ControlPoints_[1] + ControlPoints_[3]) / 2.0f) + Vector2(-thirdPointPos_.x, thirdPointPos_.y);
	//}
	//else {
	//	ControlPoints_[2] = ((ControlPoints_[1] + ControlPoints_[3]) / 2.0f) + Vector2(thirdPointPos_.x, thirdPointPos_.y);
	//}

	//
}

void TitleScene::Debug(){
#ifdef _DEBUG
	titleDirection_->Debug();

#endif // 
}
