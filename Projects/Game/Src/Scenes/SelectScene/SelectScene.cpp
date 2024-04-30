#include "SelectScene.h"
#include"Drawers/DrawerManager.h"
#include"Scenes/SelectToGame/SelectToGame.h"

SelectScene::SelectScene():
	BaseScene{ BaseScene::ID::StageSelect }{


}

void SelectScene::Initialize() {

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	currentCamera_->pos = { 0.0f, 0.0f , -1.0f };

	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i] = std::make_unique<Tex2DState>();
		texies_[i]->transform.scale = { 624.0f,368.0f };
		texies_[i]->transform.translate = { stageInterbal * i, 0.0f ,0 };
		texies_[i]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/stageSelectFrame.png");		
	}
	selectTex_ = std::make_unique<Tex2DState>();
	selectTex_->transform.scale = { 452.0f,72.0f };
	selectTex_->transform.translate = { 0.0f, 282.0f ,0 };
	selectTex_->uvTrnasform.translate = { 0.f, 0.01f };
	selectTex_->uvTrnasform.scale.x = { 0.1f };
	selectTex_->color = 0xffffffff;
	selectTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/stageNumber.png");

	selectBGM_ = audioManager_->Load("./Resources/Sounds/BGM/StageSelect.mp3");

	gameDecision_ = audioManager_->Load("./Resources/Sounds/SE/choice.mp3");
	selectMove_ = audioManager_->Load("./Resources/Sounds/SE/selectMove.mp3");
	cancel_ = audioManager_->Load("./Resources/Sounds/SE/cancel.mp3");

	selectBGM_->Start(0.1f, true);
}

void SelectScene::Finalize(){

}

void SelectScene::Update(){
	Debug();
	SelectToGame::GetInstance()->Debug();

	currentCamera_->Debug("カメラ");
	currentCamera_->Update();

	SelectMove();

	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i]->color = 0xffffff88;
		texies_[selectNum_]->color = 0xff0000ff;
		texies_[i]->transform.translate.x = (stageInterbal * i) - (stageInterbal * selectNum_);
		texies_[i]->transform.CalcMatrix();
	}

	selectTex_->transform.CalcMatrix();
	selectTex_->uvTrnasform.translate.x = 0.1f * selectNum_;
	selectTex_->uvTrnasform.CalcMatrix();
}

void SelectScene::Draw(){
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
	tex2D_->Draw(selectTex_->transform.matWorld_, selectTex_->uvTrnasform.matWorld_ , currentCamera_->GetViewOthographics()
		, selectTex_->textureID, selectTex_->color, BlendType::kNormal);

	for (size_t i = 0; i < texies_.size(); i++) {
		
		tex2D_->Draw(texies_[i]->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
			, texies_[i]->textureID, texies_[i]->color, BlendType::kNormal);
	}

	UIEditor::GetInstance()->putDraw(currentCamera_->GetViewOthographics());
}

void SelectScene::Debug(){
	ImGui::Begin("セレクト画面");
	ImGui::DragFloat2("Transform", &selectTex_->transform.translate.x, 1.0f);
	ImGui::DragFloat2("Scale", &selectTex_->transform.scale.x, 1.0f);
	ImGui::DragFloat2("UVTransform", &selectTex_->uvTrnasform.translate.x, 0.01f);
	ImGui::DragFloat2("UVScale", &selectTex_->uvTrnasform.scale.x, 0.01f);
	ImGui::End();
}

void SelectScene::SelectMove(){
	if (input_->GetKey()->Pushed(DIK_A)){
		if (selectNum_>0){
			selectMove_->Start(0.1f, false);
			selectNum_--;
		}		
	}
	else if (input_->GetKey()->Pushed(DIK_D)) {
		if (selectNum_ < texies_.size() - 1) {
			selectMove_->Start(0.1f, false);
			selectNum_++;
		}		
	}

	if (input_->GetKey()->LongPush(DIK_LSHIFT)&& input_->GetKey()->Pushed(DIK_SPACE)){
		SelectToGame::GetInstance()->SetSelect(selectNum_);
		gameDecision_->Start(0.1f, false);
		selectBGM_->Stop();
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
	if (input_->GetKey()->LongPush(DIK_LSHIFT) && input_->GetKey()->Pushed(DIK_BACKSPACE)) {
		cancel_->Start(0.1f, false);
		selectBGM_->Stop();
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}

}

