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
		texies_[i]->transform.scale *= 64.0f;
		texies_[i]->transform.translate = { -450.0f + stageInterbal * i, 0.0f ,0 };
		texies_[i]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/enemy_popEffect.png");		
	}

	selectTex_ = std::make_unique<Tex2DState>();
	selectTex_->color = 0xffffffff;
	selectTex_->transform.scale *= 64.0f;
	selectTex_->transform.translate = { texies_[0]->transform.translate.x, 100.0f ,0 };
	selectTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/arrow.png");
}

void SelectScene::Finalize(){

}

void SelectScene::Update(){
	SelectToGame::GetInstance()->Debug();

	currentCamera_->Debug("カメラ");
	currentCamera_->Update();


	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i]->color = 0xffffffff;
		texies_[selectNum_]->color = 0xff0000ff;
		texies_[i]->transform.CalcMatrix();
	}
	SelectMove();

	selectTex_->transform.translate.x = -450.0f + stageInterbal * selectNum_;
	selectTex_->transform.CalcMatrix();
}

void SelectScene::Draw(){
	tex2D_->Draw(selectTex_->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
		, selectTex_->textureID, selectTex_->color, BlendType::kNormal);

	for (size_t i = 0; i < texies_.size(); i++) {
		tex2D_->Draw(texies_[i]->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
			, texies_[i]->textureID, texies_[i]->color, BlendType::kNormal);
	}
}

void SelectScene::Debug(){

}

void SelectScene::SelectMove(){
	if (input_->GetKey()->Pushed(DIK_A)){
		if (selectNum_>0){
			selectNum_--;
		}		
	}
	else if (input_->GetKey()->Pushed(DIK_D)) {
		if (selectNum_ < texies_.size() - 1) {
			selectNum_++;
		}		
	}

	if (input_->GetKey()->Pushed(DIK_SPACE)){
		SelectToGame::GetInstance()->SetSelect(selectNum_);
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}

}

