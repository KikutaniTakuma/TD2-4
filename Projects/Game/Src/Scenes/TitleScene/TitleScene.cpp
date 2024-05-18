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

	catmullRom_ = std::make_unique<CatmullRomEditor>();
	catmullRom_->Initialize();

	moveSpeeds_.resize(3);

	moveSpeeds_[0] = 0.01f;
	moveSpeeds_[1] = 0.01f;
	moveSpeeds_[2] = 0.01f;

	ControlPoints_ = catmullRom_->GetControlPoints();

	currentCamera_->pos = { 0.f, 0.f ,-1.0f };

	titleBGM_ = audioManager_->Load("./Resources/Sounds/BGM/title.mp3");
	beginGame_ = audioManager_->Load("./Resources/Sounds/SE/choice.mp3");
	titleBGM_->Start(0.1f, true);


	runTexture_= std::make_unique<Tex2DState>();
	runTexture_->color = 0x000000ff;
	runTexture_->transform.scale = { 50.0f, 50.0f };
	runTexture_->transform.translate = { 10.0f, 10.0f };
	runTexture_->uvTrnasform.scale = { 0.0f, 0.0f };
	runTexture_->uvTrnasform.translate = { 0.0f, 0.0f };
	runTexture_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/enemy_popEffect.png");
	runTexture_->textureFullPath = "./Resources/enemy_popEffect.png";
	runTexture_->textureName = "enemy_popEffect";

}

void TitleScene::Finalize(){
	titleBGM_->Stop();

}

void TitleScene::Update()
{
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();

	auto* const key = input_->GetKey();


	if (key->Pushed(DIK_SPACE)) {
		beginGame_->Start(0.1f, false);
		sceneManager_->SceneChange(BaseScene::ID::StageSelect);
	}

	catmullRom_->Update();
	ControlPoints_ = catmullRom_->GetControlPoints();
	while (ControlPoints_.size() - 1 > moveSpeeds_.size()){
		moveSpeeds_.push_back(0.01f);
	}
	while (ControlPoints_.size() - 1 < moveSpeeds_.size()) {
		moveSpeeds_.pop_back();
	}
	//SetControlPoint();

	MoveTexture();


	runTexture_->transform.CalcMatrix();
	
	Debug();

}

void TitleScene::Draw(){

	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());

	tex2D_->Draw(runTexture_->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
		, runTexture_->textureID, runTexture_->color, BlendType::kNormal);

	catmullRom_->Draw(currentCamera_->GetViewOthographics());

#ifdef _DEBUG

	UIEditor::GetInstance()->PutDraw(currentTexCamera_->GetViewOthographics());
#endif // _DEBUG
}


void TitleScene::MoveTexture(){
	if (isMove_) {
		t_ += moveSpeeds_[linePass_];
	}
	if (t_ >= 1.0f) {
		t_ = 0.0f;
		linePass_++;
	}

	if (linePass_ == 0) {
		runTexture_->transform.translate = Vector2::CatmullRom(ControlPoints_[0], ControlPoints_[0],
			ControlPoints_[1], ControlPoints_[2], t_);
	}
	else if (linePass_ >= 1 && linePass_ < catmullRom_->GetLastLinePass()) {
		runTexture_->transform.translate = Vector2::CatmullRom(ControlPoints_[linePass_ - 1], ControlPoints_[linePass_],
			ControlPoints_[linePass_ + 1], ControlPoints_[linePass_ + 2], t_);
	}
	else if (linePass_ == catmullRom_->GetLastLinePass()) {
		runTexture_->transform.translate = Vector2::CatmullRom(ControlPoints_[catmullRom_->GetLastLinePass() - 1], ControlPoints_[catmullRom_->GetLastLinePass()],
			ControlPoints_[catmullRom_->GetLastLinePass() + 1], ControlPoints_[catmullRom_->GetLastLinePass() + 1], t_);
	}
	else {
		isMove_ = false;
	}




}


void TitleScene::SetControlPoint(){
	//第一ポイントの位置によって+-を変更
	//画面の左側なら
	if (ControlPoints_[0].x < ControlPoints_[3].x) {
		ControlPoints_[1] = ControlPoints_[0] + Vector2(-secondPointPos_.x, secondPointPos_.y);
	}
	else {
		ControlPoints_[1] = ControlPoints_[0] + Vector2(secondPointPos_.x, secondPointPos_.y);
	}

	if (ControlPoints_[0].x < ControlPoints_[3].x) {
		ControlPoints_[2] = ((ControlPoints_[1] + ControlPoints_[3]) / 2.0f) + Vector2(-thirdPointPos_.x, thirdPointPos_.y);
	}
	else {
		ControlPoints_[2] = ((ControlPoints_[1] + ControlPoints_[3]) / 2.0f) + Vector2(thirdPointPos_.x, thirdPointPos_.y);
	}

	
}

void TitleScene::Debug(){
#ifdef _DEBUG
	catmullRom_->Debug();

	ImGui::Begin("帽子の動き");

	if (ImGui::Button("テクスチャを動かす")) {
		isMove_ = true;
		linePass_ = 0;
	}
	for (size_t i = 0; i < moveSpeeds_.size(); i++){
		ImGui::DragFloat(("第" + std::to_string(i + 1) + "区間のテクスチャの動く速度").c_str(), &moveSpeeds_[i], 0.001f, 0.001f, 0.2f);
	}
	ImGui::Text("LinePass = %d", linePass_);

	ImGui::End();

#endif // 
}
