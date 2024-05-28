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

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	currentCamera_->pos = { 0.0f, 0.0f , -1.0f };
	shakePower_ = { 20.0f,20.0f };

	selectNum_ = SelectToGame::GetInstance()->GetSelect();

	for (uint32_t i = 0; i < texies_.size(); i++) {
		texies_[i] = std::make_unique<Tex2DState>();
		texies_[i]->transform.scale = { 936.0f,554.0f };
		texies_[i]->transform.translate = { stageInterbal * i, 0.0f ,0 };
		texies_[i]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/stageSelectFram2.png");
		startPos_[i] = texies_[i]->transform.translate.x;
		endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

		itemTexies_[i] = std::make_unique<Tex2DState>();
		itemTexies_[i]->transform.scale = { 80.0f,80.0f };
		itemTexies_[i]->transform.translate = { itemDistanceCenter_.x + (stageInterbal * i), itemDistanceCenter_.y ,0 };
		itemTexies_[i]->uvTransform.scale = { 0.5f,1.0f };
		itemTexies_[i]->textureID = SelectToGame::GetInstance()->GetStageItemTextureID(i);
		startItemPos_[i] = texies_[i]->transform.translate.x;
		endItemPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);
	}

	for (uint32_t i = 0; i < 3; i++){
		potNumberTexture_[i] = std::make_unique<Tex2DState>();
		potNumberTexture_[i]->transform.scale = { 60.0f,60.0f };
		potNumberTexture_[i]->transform.translate = { 70.0f * i,-300.0f };
		potNumberTexture_[i]->uvTransform.scale = { 0.1f,1.0f };
		potNumberTexture_[i]->color = 0xffffffff;
		potNumberTexture_[i]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/Timer/timeLimitNumber.png");

		timerNumberTexture_[i] = std::make_unique<Tex2DState>();
		timerNumberTexture_[i]->transform.scale = { 60.0f,60.0f };
		timerNumberTexture_[i]->transform.translate = { 70.0f * i,-227.0f };
		timerNumberTexture_[i]->uvTransform.scale = { 0.1f,1.0f };
		timerNumberTexture_[i]->color = 0xffffffff;
		timerNumberTexture_[i]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/Timer/timeLimitNumber.png");

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

	ease_.Start(false, kAddEase_, Easeing::InSine);

	for (uint32_t i = 0; i < kMaxStage_; i++){
		LoadGameData(i);
	}
	

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

	if (coolTime_!=0){
		coolTime_--;
	}

	CalcUVPos(inGameDatas_[selectNum_].timeLimit, timerNumberTexture_);
	CalcUVPos(inGameDatas_[selectNum_].clearItemNum, potNumberTexture_);

	for (uint32_t i = 0; i < 3; i++){
		timerNumberTexture_[i]->transform.CalcMatrix();
		timerNumberTexture_[i]->uvTransform.CalcMatrix();
		potNumberTexture_[i]->transform.CalcMatrix();
		potNumberTexture_[i]->uvTransform.CalcMatrix();
	}
	

	ease_.Update();

	for (size_t i = 0; i < texies_.size(); i++) {
		texies_[i]->color = 0xffffff88;
		texies_[selectNum_]->color = 0xffffffff;
		itemTexies_[i]->color = 0xffffff88;
		itemTexies_[selectNum_]->color = 0xffffffff;

		const auto& clearFlug = SelectToGame::GetInstance()->GetClearFlug();

		if (clearFlug[i]) {
			itemTexies_[i]->uvTransform.translate.x = 0.5f;
		}

		

		texies_[i]->transform.translate.x = ease_.Get(startPos_[i], endPos_[i]);
		itemTexies_[i]->transform.translate.x = ease_.Get(startItemPos_[i], endItemPos_[i]) + itemDistanceCenter_.x;
		itemTexies_[i]->transform.translate.y = itemDistanceCenter_.y;

		texies_[i]->transform.CalcMatrix();
		itemTexies_[i]->transform.CalcMatrix();
		itemTexies_[i]->uvTransform.CalcMatrix();
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

		tex2D_->Draw(itemTexies_[i]->transform.matWorld_, itemTexies_[i]->uvTransform.matWorld_, currentCamera_->GetViewOthographics()
			, itemTexies_[i]->textureID, itemTexies_[i]->color, BlendType::kNormal);
	}

	for (size_t i = 0; i < 3; i++) {
		if (ease_.GetIsActive()){
			UIEditor::GetInstance()->SetSelectDraw(true);
			return;
		}
		else {
			UIEditor::GetInstance()->SetSelectDraw(false);
		}
		tex2D_->Draw(potNumberTexture_[i]->transform.matWorld_, potNumberTexture_[i]->uvTransform.matWorld_, currentCamera_->GetViewOthographics()
			, potNumberTexture_[i]->textureID, potNumberTexture_[i]->color, BlendType::kNormal);

		tex2D_->Draw(timerNumberTexture_[i]->transform.matWorld_, timerNumberTexture_[i]->uvTransform.matWorld_, currentCamera_->GetViewOthographics()
			, timerNumberTexture_[i]->textureID, timerNumberTexture_[i]->color, BlendType::kNormal);
	}

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
	ImGui::DragFloat3("ポットの百の位", potNumberTexture_[0]->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("ポットの十の位", potNumberTexture_[1]->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("ポットの一の位", potNumberTexture_[2]->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("タイマーの百の位", timerNumberTexture_[0]->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("タイマーの十の位", timerNumberTexture_[1]->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("タイマーの一の位", timerNumberTexture_[2]->transform.translate.data(), 0.1f);

	ImGui::DragFloat2("アイテムの相対位置", itemDistanceCenter_.data(), 1.0f);
	ImGui::End();
#endif // _DEBUG
}

void SelectScene::SelectMove(){
	float stick = input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X, 0.5f);

	if (input_->GetKey()->LongPush(DIK_A) || input_->GetKey()->LongPush(DIK_LEFT)
		|| input_->GetGamepad()->GetButton(Gamepad::Button::LEFT) || 0.0f > stick) {
		if (selectNum_>0 && coolTime_ == 0){
			coolTime_ = kCoolTime_;
			selectMove_->Start(0.1f, false);
			selectNum_--;
			ease_.Start(false, kAddEase_, Easeing::InSine);
			for (size_t i = 0; i < kMaxStage_; i++){
				startPos_[i] = texies_[i]->transform.translate.x;
				endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

				startItemPos_[i] = texies_[i]->transform.translate.x;
				endItemPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);
			}
		
		}		
	}
	else if (input_->GetKey()->LongPush(DIK_D) || input_->GetKey()->LongPush(DIK_RIGHT)
		|| input_->GetGamepad()->GetButton(Gamepad::Button::RIGHT) || 0.0f < stick) {

		if (selectNum_ < texies_.size() - 1 && coolTime_ == 0) {
			coolTime_ = kCoolTime_;
			selectMove_->Start(0.1f, false);
			selectNum_++;
			ease_.Start(false, kAddEase_, Easeing::InSine);
			for (size_t i = 0; i < kMaxStage_; i++) {
				startPos_[i] = texies_[i]->transform.translate.x;
				endPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);

				startItemPos_[i] = texies_[i]->transform.translate.x;
				endItemPos_[i] = (stageInterbal * i) - (stageInterbal * selectNum_);
			}
		}		
	}

	if ((input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->GetButton(Gamepad::Button::A)) && !sceneManager_->GetFadeActive()) {
		SelectToGame::GetInstance()->SetSelect(selectNum_);
		gameDecision_->Start(0.2f, false);
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
	if ((input_->GetKey()->Pushed(DIK_BACKSPACE) || input_->GetGamepad()->GetButton(Gamepad::Button::B)) && !sceneManager_->GetFadeActive()) {
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

	inGameDatas_[stageNumber].timeLimit = root[stageName]["最大時間"];
	inGameDatas_[stageNumber].clearItemNum = root[stageName]["クリアに必要なアイテムの数"];


//#ifdef _DEBUG
//	std::string message = "File loading completed";
//	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
//
//#endif // _DEBUG

}

void SelectScene::CalcUVPos(float InGameData, std::array<std::unique_ptr<Tex2DState>, 3>& uvPos)
{
	texUVPos_[0] = static_cast<int32_t>(InGameData) % 10;
	texUVPos_[1] = static_cast<int32_t>(InGameData / 10.0f) % 10;
	texUVPos_[2] = static_cast<int32_t>(InGameData / 100.0f) % 10;

	uvPos[0]->uvTransform.translate.x = texUVPos_[2] * 0.1f;
	uvPos[1]->uvTransform.translate.x = texUVPos_[1] * 0.1f;
	uvPos[2]->uvTransform.translate.x = texUVPos_[0] * 0.1f;
}



