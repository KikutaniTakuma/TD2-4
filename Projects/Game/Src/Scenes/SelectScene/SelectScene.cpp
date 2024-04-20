#include "SelectScene.h"

bool SelectScene::isStageClear_[maxStage_]{};

int SelectScene::selectNum_ = 0;

struct Tex2DState {
	Transform transform;
	uint32_t textureID;
	uitn32_t color;
};

SelectScene::SelectScene():
	BaseScene{ BaseScene::ID::StageSelect }{


}

void SelectScene::Initialize() {

	currentCamera_->pos = Vector2{ 0.f, 0.f };
	

	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i] = std::make_unique<Texture2D>("./Resources/enemy_popEffect.png");
		texies_[i]->scale *= 60.0f;
		texies_[i]->pos = Vector2{ -450.0f + stageInterbal * i, 0.0f };
	}

	selectTex_ = std::make_unique<Texture2D>("./Resources/arrow.png");
	selectTex_->scale *= 80.0f;
	selectTex_->pos = { texies_[0]->pos.x, 100.0f };
}

void SelectScene::Finalize(){

}

void SelectScene::Update(){
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();


	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i]->color = 0xffffffff;
		texies_[selectNum_]->color = 0xff0000ff;
	}
	SelectMove();

	selectTex_->pos.x = -450.0f + stageInterbal * selectNum_;

	selectTex_->Debug("テスト用サークル");
	


}

void SelectScene::Draw(){
	selectTex_->Draw(currentCamera_->GetViewOthographics());

	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i]->Draw(currentCamera_->GetViewOthographics());
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



}

