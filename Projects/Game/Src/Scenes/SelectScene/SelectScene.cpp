#include "SelectScene.h"
#include"Drawers/DrawerManager.h"
#include"Scenes/SelectToGame/SelectToGame.h"
#include"Utils/Easeing/Easeing.h"

SelectScene::SelectScene():
	BaseScene{ BaseScene::ID::StageSelect }{


}

void SelectScene::Initialize() {

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	currentCamera_->pos = { 0.0f, 0.0f , -1.0f };
	shakePower_ = { 20.0f,20.0f };

	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i] = std::make_unique<Tex2DState>();
		texies_[i]->transform.scale = { 624.0f,368.0f };
		texies_[i]->transform.translate = { stageInterbal * i, 0.0f ,0 };
		texies_[i]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/stageSelectFrame.png");
		startPos_[i] = texies_[i]->transform.translate.x;
		endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);
	}
	selectTex_ = std::make_unique<Tex2DState>();
	selectTex_->transform.scale = { 452.0f,72.0f };
	selectTex_->transform.translate = { 0.0f, 282.0f ,0 };
	selectTex_->uvTransform.translate = { 0.f, 0.01f };
	selectTex_->uvTransform.scale.x = { 0.1f };
	selectTex_->color = 0xffffffff;
	selectTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/stageNumber.png");

	selectBGM_ = audioManager_->Load("./Resources/Sounds/BGM/StageSelect.mp3");

	gameDecision_ = audioManager_->Load("./Resources/Sounds/SE/choice.mp3");
	selectMove_ = audioManager_->Load("./Resources/Sounds/SE/selectMove.mp3");
	cancel_ = audioManager_->Load("./Resources/Sounds/SE/cancel.mp3");

	selectBGM_->Start(0.1f, true);
}

void SelectScene::Finalize(){
	if (selectBGM_) {
		selectBGM_->Stop();
	}
}

void SelectScene::Update(){
	Debug();
	SelectToGame::GetInstance()->Debug();

	currentCamera_->Debug("カメラ");
	currentCamera_->Shake(1.0f);
	currentCamera_->Update();

	SelectMove();

	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i]->color = 0xffffff88;
		texies_[selectNum_]->color = 0xff0000ff;
		
	}
	if (coolTime_!=0){
		coolTime_--;
	}
	

	ease_.Update();

	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i]->transform.translate.x = ease_.Get(startPos_[i], endPos_[i]);

		texies_[i]->transform.CalcMatrix();
	}

	selectTex_->transform.CalcMatrix();
	selectTex_->uvTransform.translate.x = 0.1f * selectNum_;
	selectTex_->uvTransform.CalcMatrix();
}

void SelectScene::Draw(){
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
	tex2D_->Draw(selectTex_->transform.matWorld_, selectTex_->uvTransform.matWorld_ , currentCamera_->GetViewOthographics()
		, selectTex_->textureID, selectTex_->color, BlendType::kNormal);

	for (size_t i = 0; i < texies_.size(); i++) {
		
		tex2D_->Draw(texies_[i]->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
			, texies_[i]->textureID, texies_[i]->color, BlendType::kNormal);
	}

#ifdef _DEBUG

	UIEditor::GetInstance()->PutDraw(currentTexCamera_->GetViewOthographics());
#endif // _DEBUG
}

void SelectScene::Debug(){
#ifdef _DEBUG
	ImGui::Begin("セレクト画面");
	if (ImGui::Button("シェイクテスト")){
		currentCamera_->BeginShake(shakePower_);
	}
	ImGui::End();
#endif // _DEBUG
}

void SelectScene::SelectMove(){
	if (input_->GetKey()->LongPush(DIK_A)|| input_->GetKey()->LongPush(DIK_LEFT)||input_->GetGamepad()->GetButton(Gamepad::Button::LEFT)) {
		if (selectNum_>0 && coolTime_ == 0){
			coolTime_ = kCoolTime_;
			selectMove_->Start(0.1f, false);
			selectNum_--;
			ease_.Start(false, kAddEase_, Easeing::InSine);
			for (size_t i = 0; i < kMaxStage_; i++){
				startPos_[i] = texies_[i]->transform.translate.x;
				endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);
			}
		
		}		
	}
	else if (input_->GetKey()->LongPush(DIK_D) || input_->GetKey()->LongPush(DIK_RIGHT) || input_->GetGamepad()->GetButton(Gamepad::Button::RIGHT)) {
		if (selectNum_ < texies_.size() - 1 && coolTime_ == 0) {
			coolTime_ = kCoolTime_;
			selectMove_->Start(0.1f, false);
			selectNum_++;
			ease_.Start(false, kAddEase_, Easeing::InSine);
			for (size_t i = 0; i < kMaxStage_; i++) {
				startPos_[i] = texies_[i]->transform.translate.x;
				endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);
			}
		}		
	}

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->GetButton(Gamepad::Button::A)){
		SelectToGame::GetInstance()->SetSelect(selectNum_);
		gameDecision_->Start(0.2f, false);
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
	if (input_->GetKey()->Pushed(DIK_BACKSPACE)|| input_->GetGamepad()->GetButton(Gamepad::Button::B)) {
		cancel_->Start(0.2f, false);
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}

}

