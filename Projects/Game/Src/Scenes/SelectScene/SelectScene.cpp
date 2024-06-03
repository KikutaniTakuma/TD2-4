#include "SelectScene.h"
#include"Drawers/DrawerManager.h"
#include"Scenes/SelectToGame/SelectToGame.h"
#include"Utils/Easeing/Easeing.h"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include"../SoLib/SoLib/SoLib_Json.h"
#include"Input/Mouse/Mouse.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include <fstream>
#include <Utils/EngineInfo/EngineInfo.h>
#include"Error/Error.h"


SelectScene::SelectScene():
	BaseScene{ BaseScene::ID::StageSelect }{


}

void SelectScene::Initialize() {
	tex2D_ = drawerManager_->GetTexture2D();

	currentCamera_->pos = { 0.0f, 0.0f , -1.0f };
	shakePower_ = { 20.0f,20.0f };

	Lamb::SafePtr selectToGame = SelectToGame::GetInstance();

	selectNum_ = selectToGame->GetSelect();

	selectToGame->SetRetryFlug(false);

	for (uint32_t i = 0; i < frameTexies_.size(); i++) {
		frameTexies_[i] = std::make_unique<Tex2DState>();
		frameTexies_[i]->transform.scale = Vector2{ 936.0f,554.0f } * 0.7f;
		frameTexies_[i]->transform.translate = { stageInterbal * i, 0.0f ,0 };
		frameTexies_[i]->uvTransform.scale.x = 0.5f;
		frameTexies_[i]->textureID = drawerManager_->LoadTexture("./Resources/UI/stageSelectFrame.png");
		startPos_[i] = frameTexies_[i]->transform.translate.x;
		endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

		itemTexies_[i] = std::make_unique<Tex2DState>();
		itemTexies_[i]->transform.scale = Vector2{ 80.0f,80.0f } * 2.5f;
		itemTexies_[i]->transform.translate = { stageInterbal * i, -70.0f ,0 };
		itemTexies_[i]->uvTransform.scale = { 0.5f,1.0f };
		itemTexies_[i]->textureID = selectToGame->GetStageItemTextureID(i);
		startItemPos_[i] = frameTexies_[i]->transform.translate.x;
		endItemPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

		stageNumTexies_[i] = std::make_unique<Tex2DState>();
		stageNumTexies_[i]->transform.scale = Vector2{ 480.0f,80.0f };
		stageNumTexies_[i]->transform.translate = { stageNumDistance_ + (stageInterbal * i), 100.0f ,0 };
		stageNumTexies_[i]->uvTransform.scale = { 0.1f,1.0f };
		stageNumTexies_[i]->uvTransform.translate = { 0.1f * i,1.0f };
		stageNumTexies_[i]->textureID = drawerManager_->LoadTexture("./Resources/UI/stageNumber.png");
		startStageNumPos_[i] = stageNumTexies_[i]->transform.translate.x;
		endStageNumPos_[i] = stageNumDistance_ + (stageInterbal * i) - (stageInterbal * selectNum_);
	}

	selectTex_ = std::make_unique<Tex2DState>();
	selectTex_->transform.scale = { 452.0f,72.0f };
	selectTex_->transform.translate = { 0.0f, 282.0f ,0 };
	selectTex_->uvTransform.translate = { 0.f, 0.01f };
	selectTex_->uvTransform.scale.x = { 1.0f };
	selectTex_->color = 0xffffffff;
	selectTex_->textureID = drawerManager_->LoadTexture("./Resources/UI/stageSelectMessege.png");

	selectBGM_ = audioManager_->Load("./Resources/Sounds/BGM/StageSelect.mp3");

	gameDecision_ = audioManager_->Load("./Resources/Sounds/SE/choice.mp3");
	selectMove_ = audioManager_->Load("./Resources/Sounds/SE/selectMove.mp3");
	cancel_ = audioManager_->Load("./Resources/Sounds/SE/cancel.mp3");

	selectBGM_->Start(0.1f, true);

	ease_.Start(false, kAddEase_, Easeing::InSine);

	for (uint32_t i = 0; i < kMaxStage_; i++){
		LoadGameData(i);
	}
	
	ChangeScales_[0] = frameTexies_[selectNum_]->transform.scale;
	ChangeScales_[1] = itemTexies_[selectNum_]->transform.scale;
	ChangeScales_[2] = stageNumTexies_[selectNum_]->transform.scale;
}

void SelectScene::Finalize(){
	if (selectBGM_) {
		selectBGM_->Stop();
	}
}

void SelectScene::Update(){
	Lamb::SafePtr selectToGame = SelectToGame::GetInstance();

	Debug();
	selectToGame->Debug();

	currentCamera_->Debug("カメラ");
	currentCamera_->Shake(1.0f);
	currentCamera_->Update();

	SelectMove();

	if (coolTime_!=0){
		coolTime_--;
	}


	ease_.Update();

	for (size_t i = 0; i < kMaxStage_; i++) {
		frameTexies_[i]->color = 0xffffff88;
		frameTexies_[selectNum_]->color = 0xffffffff;
		itemTexies_[i]->color = 0xffffff88;
		itemTexies_[selectNum_]->color = 0xffffffff;
		stageNumTexies_[i]->color = 0xffffff88;
		stageNumTexies_[selectNum_]->color = 0xffffffff;

		const auto& clearFlug = selectToGame->GetClearFlug();

		if (clearFlug[i]) {
			itemTexies_[i]->uvTransform.translate.x = 0.5f;
		}		

		frameTexies_[i]->transform.translate.x = ease_.Get(startPos_[i], endPos_[i]);
		if (clearFlug[i]) {
			frameTexies_[i]->uvTransform.translate.x = 0.5f;
		}
		else {
			frameTexies_[i]->uvTransform.translate.x = 0.0f;
		}
		frameTexies_[i]->uvTransform.CalcMatrix();
		itemTexies_[i]->transform.translate.x = ease_.Get(startItemPos_[i], endItemPos_[i]);// +itemDistanceCenter_.x;
		stageNumTexies_[i]->transform.translate.x = ease_.Get(startStageNumPos_[i], endStageNumPos_[i]);

		//itemTexies_[i]->transform.translate.y = itemDistanceCenter_.y;

		frameTexies_[i]->transform.CalcMatrix();
		itemTexies_[i]->transform.CalcMatrix();
		itemTexies_[i]->uvTransform.CalcMatrix();
		stageNumTexies_[i]->transform.CalcMatrix();
		stageNumTexies_[i]->uvTransform.CalcMatrix();
	}

	selectTex_->transform.CalcMatrix();
	selectTex_->uvTransform.CalcMatrix();
}

void SelectScene::Draw(){
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
	

	for (size_t i = 0; i < frameTexies_.size(); i++) {
		
		tex2D_->Draw(frameTexies_[i]->transform.matWorld_, frameTexies_[i]->uvTransform.matWorld_, currentCamera_->GetViewOthographics()
			, frameTexies_[i]->textureID, frameTexies_[i]->color, BlendType::kNormal);

		tex2D_->Draw(itemTexies_[i]->transform.matWorld_, itemTexies_[i]->uvTransform.matWorld_, currentCamera_->GetViewOthographics()
			, itemTexies_[i]->textureID, itemTexies_[i]->color, BlendType::kNormal);

		tex2D_->Draw(stageNumTexies_[i]->transform.matWorld_, stageNumTexies_[i]->uvTransform.matWorld_, currentCamera_->GetViewOthographics()
			, stageNumTexies_[i]->textureID, stageNumTexies_[i]->color, BlendType::kNormal);
	}

	
	tex2D_->Draw(selectTex_->transform.matWorld_, selectTex_->uvTransform.matWorld_, currentCamera_->GetViewOthographics()
		, selectTex_->textureID, selectTex_->color, BlendType::kNormal);
	

#ifdef _DEBUG

	UIEditor::GetInstance()->PutDraw(currentCamera_->GetViewOthographics());
#endif // _DEBUG
}

void SelectScene::Debug(){
#ifdef _DEBUG
	ImGui::Begin("セレクト画面");
	if (ImGui::Button("シェイクテスト")){
		currentCamera_->BeginShake(shakePower_);
	}
	ImGui::DragFloat("ステージ番号の座標補正", &stageNumDistance_, 1.0f);

	ImGui::End();
#endif // _DEBUG
}

void SelectScene::SelectMove(){
	float stick = input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X, 0.5f);
	
	NumberShortcuts();

	if (input_->GetKey()->LongPush(DIK_A) || input_->GetKey()->LongPush(DIK_LEFT)
		|| input_->GetGamepad()->GetButton(Gamepad::Button::LEFT) || 0.0f > stick) {
		if (selectNum_>0 && coolTime_ == 0){
			coolTime_ = kCoolTime_;
			selectMove_->Start(0.1f, false);
			selectNum_--;
			ease_.Start(false, kAddEase_, Easeing::InSine);
			for (size_t i = 0; i < kMaxStage_; i++){
				startPos_[i] = frameTexies_[i]->transform.translate.x;
				endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

				startItemPos_[i] = frameTexies_[i]->transform.translate.x;
				endItemPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

				startStageNumPos_[i] = stageNumTexies_[i]->transform.translate.x;
				endStageNumPos_[i] = stageNumDistance_ + (stageInterbal * i) - (stageInterbal * selectNum_);
			}
			ChangeScales_[0] = frameTexies_[selectNum_]->transform.scale;
			ChangeScales_[1] = itemTexies_[selectNum_]->transform.scale;
			ChangeScales_[2] = stageNumTexies_[selectNum_]->transform.scale;
		
		}		
	}
	else if (input_->GetKey()->LongPush(DIK_D) || input_->GetKey()->LongPush(DIK_RIGHT)
		|| input_->GetGamepad()->GetButton(Gamepad::Button::RIGHT) || 0.0f < stick) {

		if (selectNum_ < frameTexies_.size() - 1 && coolTime_ == 0) {
			coolTime_ = kCoolTime_;
			selectMove_->Start(0.1f, false);
			selectNum_++;
			ease_.Start(false, kAddEase_, Easeing::InSine);
			for (size_t i = 0; i < kMaxStage_; i++) {
				startPos_[i] = frameTexies_[i]->transform.translate.x;
				endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

				startItemPos_[i] = frameTexies_[i]->transform.translate.x;
				endItemPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

				startStageNumPos_[i] = stageNumTexies_[i]->transform.translate.x;
				endStageNumPos_[i] = stageNumDistance_ + (stageInterbal * i) - (stageInterbal * selectNum_);
			}
			ChangeScales_[0] = frameTexies_[selectNum_]->transform.scale;
			ChangeScales_[1] = itemTexies_[selectNum_]->transform.scale;
			ChangeScales_[2] = stageNumTexies_[selectNum_]->transform.scale;

		}
	}

	if ((input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->GetButton(Gamepad::Button::A)) && !sceneManager_->GetFadeActive() && not ease_.GetIsActive()) {
		SelectToGame::GetInstance()->SetSelect(selectNum_);
		gameDecision_->Start(0.2f, false);
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
	if ((input_->GetKey()->Pushed(DIK_ESCAPE) || input_->GetGamepad()->GetButton(Gamepad::Button::B)) && !sceneManager_->GetFadeActive()) {
		cancel_->Start(0.2f, false);
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}

}

void SelectScene::LoadGameData(const uint32_t stageNumber){
	std::string stageName = (kItemName_ + std::to_string(stageNumber)).c_str();
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + stageName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		throw Lamb::Error::Code<SelectScene>(message, ErrorPlace);
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループを検索
	nlohmann::json::iterator itGroup = root.find(stageName);
	//未登録チェック
	assert(itGroup != root.end());



//#ifdef _DEBUG
//	std::string message = "File loading completed";
//	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
//
//#endif // _DEBUG

}

void SelectScene::NumberShortcuts(){
	const auto key = input_->GetKey();

	if (key->Pushed(DIK_1)) {
		PlayerSelectNumberMove(DIK_1);
	}
	else if (key->Pushed(DIK_2)) {
		PlayerSelectNumberMove(DIK_2);
	}
	else if (key->Pushed(DIK_3)) {
		PlayerSelectNumberMove(DIK_3);
	}
	else if (key->Pushed(DIK_4)) {
		PlayerSelectNumberMove(DIK_4);
	}
	else if (key->Pushed(DIK_5)) {
		PlayerSelectNumberMove(DIK_5);
	}
	else if (key->Pushed(DIK_6)) {
		PlayerSelectNumberMove(DIK_6);
	}
	else if (key->Pushed(DIK_7)) {
		PlayerSelectNumberMove(DIK_7);
	}
	else if (key->Pushed(DIK_8)) {
		PlayerSelectNumberMove(DIK_8);
	}
}

void SelectScene::PlayerSelectNumberMove(const uint32_t PushedNumber){
	if (PushedNumber < 2 || PushedNumber > 9) {
		return;
	}
	selectNum_ = PushedNumber - 2;
	coolTime_ = kCoolTime_;
	selectMove_->Start(0.1f, false);
	ease_.Start(false, kAddEase_, Easeing::InSine);
	for (size_t i = 0; i < kMaxStage_; i++) {
		startPos_[i] = frameTexies_[i]->transform.translate.x;
		endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

		startItemPos_[i] = frameTexies_[i]->transform.translate.x;
		endItemPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

		startStageNumPos_[i] = stageNumTexies_[i]->transform.translate.x;
		endStageNumPos_[i] = stageNumDistance_ + (stageInterbal * i) - (stageInterbal * selectNum_);
	}

	ChangeScales_[0] = frameTexies_[selectNum_]->transform.scale;
	ChangeScales_[1] = itemTexies_[selectNum_]->transform.scale;
	ChangeScales_[2] = stageNumTexies_[selectNum_]->transform.scale;

}




