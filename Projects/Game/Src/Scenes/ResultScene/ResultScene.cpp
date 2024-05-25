#include "ResultScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"
#include "Game/Water/Water.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include "Utils/Random/Random.h"

#include"Scenes/SelectToGame/SelectToGame.h"
#include "Game/GameManager/GameManager.h"

bool ResultScene::isGameClear_ = false;

ResultScene::ResultScene():
BaseScene{ BaseScene::ID::Result }
{
}

void ResultScene::Initialize(){
	// ゲームのクリア状況入力
	isGameClear_ = true;
	
	// ステージ番号入力
	preGameStageNumber_ = static_cast<uint32_t>(SelectToGame::GetInstance()->GetSelect());



	effectStatus_ = EffectState::kFirst;

	currentCamera_->pos = { 0.f, 0.f ,-10.0f };

	tex2D_ = drawerManager_->GetTexture2D();
	backGround_ = std::make_unique<Tex2DState>();
	backGround_->transform.translate.z = 50.0f;
	backGround_->transform.scale = Lamb::ClientSize();
	rotateClearBackFround_ = 0.0f;

	resultMessage_ = std::make_unique<Tex2DState>();
	resultMessage_->transform.scale = { 600.0f, 150.0f, 0.0f };
	resultMessage_->transform.translate = { 25.0f, -180.0f, 0.0f };
	resultMessageEase_ = std::make_unique<Easeing>();
	resultMessageYPos_ = { resultMessage_->transform.translate.y,resultMessage_->transform.translate.y + 60.0f };

	if (isGameClear_) {
		backGround_->textureID = drawerManager_->LoadTexture("./Resources/BackGround/clearBackGround.png");
		resultMessage_->textureID = drawerManager_->LoadTexture("./Resources/Result/clearUi.png");
		currentUIPick_ = CurrentUIPick::kToNext;

		clearItem_ = std::make_unique<Tex2DState>();
		clearItem_->transform.translate.z = -5.0f;
		clearItem_->textureID = drawerManager_->LoadTexture("./Resources/Result/product.png");
		clearItemEase_ = std::make_unique<Easeing>();
		clearItemYPos_ = { 60.0f, 130.f };
		clearItemScaleDuration_ = { Vector3::kIdentity * 30.0f, Vector3::kIdentity * 120.0f };
		clearItemParticle_ = std::make_unique<Particle>();
		clearItemParticle_->LoadSettingDirectory("MagicHand");
	}
	else {
		backGround_->textureID = drawerManager_->LoadTexture("./Resources/BackGround/gameOverBackGround.png");
		resultMessage_->textureID = drawerManager_->LoadTexture("./Resources/Result/gameOverUi.png");
		currentUIPick_ = CurrentUIPick::kRetry;
	}
		zanennTexID_ = drawerManager_->LoadTexture("./Resources/Result/gameOverEffect.png");
	//clearTextureID_ = drawerManager_->LoadTexture("./Resources/BackGround/gameOverBackGround.png");

	cauldronParticle_ = std::make_unique<Particle>();
	if (isGameClear_) {
		cauldronParticle_->LoadSettingDirectory("ResultExplosion");
		cauldronParticle_->SetParticleScale(1.0f / 0.0036f * 0.5f);
	}
	else {
		cauldronParticle_->LoadSettingDirectory("Bomb");
	}
	cauldronTextureID_ = drawerManager_->LoadTexture("./Resources/UI/pot.png");
	cauldronTransform_ = std::make_unique<Transform>();
	cauldronBasisPos_ = Vector3::kYIdentity * -128.0f;
	cauldronShake_.first = (Vector3::kXIdentity + Vector3::kYIdentity) * -20.0f;
	cauldronShake_.second = (Vector3::kXIdentity + Vector3::kYIdentity) * 20.0f;
	cauldronScale_.first = (Vector3::kXIdentity + Vector3::kYIdentity) * 450.0f;
	cauldronScale_.second = cauldronScale_.first * 1.5f;
	cauldronEase_ = std::make_unique<Easeing>();


	flaskTextureID_[0] = drawerManager_->LoadTexture("./Resources/Item/lizardTail.png");
	flaskTextureID_[1] = drawerManager_->LoadTexture("./Resources/Item/water.png");
	flaskTextureID_[2] = drawerManager_->LoadTexture("./Resources/Item/herbs.png");
	flaskTextureID_[3] = drawerManager_->LoadTexture("./Resources/Item/mineral.png");
	uint32_t currentElementType = static_cast<uint32_t>(Block::BlockType::kRed);

	flaskParticleAppDurationMin = { 0.3f, 0.4f };
	flaskParticleAppDurationMax = { 0.05f, 0.1f };
	flaskParticleAppDurationEase_ = std::make_unique<Easeing>();

	for (auto texID = flaskTextureID_.begin(); auto & i : flaskParticles_) {
		i = std::make_unique<FlaskParticle>();
		i->SetParticleSize(Vector3::kIdentity * 50.0f, Vector3::kIdentity * 80.0f);

		// ここでゲームプレイ中のデータを入れる予定
		i->Resize(/*GameManager::GetInstance()->GetItemTypeCount(static_cast<Block::BlockType>(currentElementType))*/10);
		currentElementType++;
		allFlaskParticleNum_ += static_cast<float>(i->GetSize());


		i->SetDeathTime({1.0f, 1.5f});
		i->SetRotate(Vector2(15_deg, 165_deg));
		i->SetRadius(Vector2(300.0f, 400.0f));
		i->SetFreq(Vector2(flaskParticleAppDurationMin.min, flaskParticleAppDurationMin.max));
		i->SetEndTranslate(Vector3::kYIdentity * 60.0f);
		i->SetTextureID(*texID);
		i->Start();
		texID++;
		if (texID == flaskTextureID_.end()) {
			break;
		}
	}
	backGroundStartPos_ = backGround_->transform.translate;
	backGroundStartPos_.y = Lamb::ClientSize().y;
	backGroundEndPos_ = backGround_->transform.translate;

	backGroundEase_ = std::make_unique<Easeing>();

	isFirstActive_ = true;

	toNextUI_  = std::make_unique<Tex2DState>();
	retryUI_ = std::make_unique<Tex2DState>();
	toStageSelectUI_ = std::make_unique<Tex2DState>();

	toNextUI_->textureID = drawerManager_->LoadTexture("./Resources/Result/next.png");
	retryUI_->textureID = drawerManager_->LoadTexture("./Resources/Result/retry.png");
	toStageSelectUI_->textureID = drawerManager_->LoadTexture("./Resources/Result/stageSelect.png");

	toNextUI_->uvTransform.scale.x = 0.5f;
	retryUI_->uvTransform.scale.x = 0.5f;
	toStageSelectUI_->uvTransform.scale.x = 0.5f;

	toNextUI_->transform.scale = { 256.0f, 72.0f, 1.0f };
	retryUI_->transform.scale = { 256.0f, 72.0f, 1.0f };
	toStageSelectUI_->transform.scale = { 256.0f, 72.0f, 1.0f };

	toNextUI_->transform.translate = { 470.0f, -150.0f, 0.0f };
	retryUI_->transform.translate = { 470.0f, -222.0f, 0.0f };
	toStageSelectUI_->transform.translate = { 470.0f, -294.0f, 0.0f };

	witch_ = std::make_unique<Tex2DState>();
	witch_->transform.scale *= 400.0f;
	witch_->transform.translate.z = 0.001f;
	witch_->transform.translate.y = 200.0f;
	witchAnimator_ = std::make_unique<Tex2DAniamtor>();
	witchAnimator_->SetDuration(0.25f);
	witchAnimator_->SetLoopAnimation(true);
	witchAnimator_->SetAnimationNumber(4);
	witchResultAnimator_ = std::make_unique<Tex2DAniamtor>();
	witchResultAnimator_->SetDuration(1.0f);
	witchResultAnimator_->SetLoopAnimation(true);
	witchResultAnimator_->SetAnimationNumber(2);

	witchMoveX_ = std::make_unique<Easeing>();
	witchMoveY_ = std::make_unique<Easeing>();
	witchMoveY2_ = std::make_unique<Easeing>();

	witchCryTexID_ = drawerManager_->LoadTexture("./Resources/Result/witchCry.png");
	witchHappyTexID_ = drawerManager_->LoadTexture("./Resources/Result/witchHappy.png");
	witchFlyAwayTexID_ = drawerManager_->LoadTexture("./Resources/Result/witchFlyAway.png");
	witchCraftTexID_ = drawerManager_->LoadTexture("./Resources/Result/witchCraft.png");

	gameDecision_ = audioManager_->Load("./Resources/Sounds/SE/choice.mp3");
	selectMove_ = audioManager_->Load("./Resources/Sounds/SE/selectMove.mp3");

	witchCraft_ = audioManager_->Load("./Resources/Sounds/SE/craft.mp3");
	witchGameOverBGM_ = audioManager_->Load("./Resources/Sounds/SE/gameOver.mp3");
	witchGameClearBGM_ = audioManager_->Load("./Resources/Sounds/SE/gameClear.mp3");
	witchCraftExplotion_ = audioManager_->Load("./Resources/Sounds/SE/craftExplosion.mp3");
}

void ResultScene::Finalize(){
	witchGameOverBGM_->Stop();
	witchGameClearBGM_->Stop();
}

void ResultScene::Update(){
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();


	switch (effectStatus_)
	{
	case ResultScene::EffectState::kFirst:
		FirstEffect();
		break;
	case ResultScene::EffectState::kSecond:
		SecondEffect();
		break;
	case ResultScene::EffectState::kGameOver:
		GameOverEffect();
		break;
	case ResultScene::EffectState::kGameClear:
		GameClearEffect();
		break;
	default:
		throw Lamb::Error::Code<ResultScene>("effectStatus is abnormal condition", ErrorPlace);
		break;
	}


	if (effectStatus_ == ResultScene::EffectState::kFirst and isFirstEnd_) {
		effectStatus_ = ResultScene::EffectState::kSecond;
		SecondEffect();
	}
	if (isFirstActive_.OnExit()) {
		if (isGameClear_) {
			effectStatus_ = EffectState::kGameClear;
			isSkip_ = true;
			GameClearEffect();
		}
		// もしゲームオーバーなら
		else {
			effectStatus_ = EffectState::kGameOver;
			isSkip_ = true;
			GameOverEffect();
		}

		witchCraft_->Stop();

		cauldronParticle_->ParticleStart();
		witchCraftExplotion_->Start(0.2f, false);
		cauldronParticle_->emitterPos.z = currentCamera_->pos.z + 1.0f;
	}

	cauldronParticle_->Update();

	if (cauldronParticle_->GetIsParticleStart().OnExit()) {
		backGroundEase_->Start(
			false,
			0.8f,
			Easeing::OutBounce
		);
	}
	
	cauldronTransform_->CalcMatrix();
	Skip();
}

void ResultScene::Draw(){
	DrawConstantUI();

	tex2D_->Draw(
		cauldronTransform_->matWorld_,
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		cauldronTextureID_,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNormal
	);
	
	switch (effectStatus_)
	{
	case ResultScene::EffectState::kFirst:
		FirstDraw();
		break;
	case ResultScene::EffectState::kSecond:
		SecondDraw();
		break;
	case ResultScene::EffectState::kGameOver:
		GameOverDraw();
		break;
	case ResultScene::EffectState::kGameClear:
		GameClearDraw();
		break;
	default:
		throw Lamb::Error::Code<ResultScene>("effectStatus is abnormal condition", ErrorPlace);
		break;
	}


	cauldronParticle_->Draw(currentCamera_->rotate, currentCamera_->GetViewOthographics());
}

void ResultScene::Debug(){

}

void ResultScene::FirstEffect() {
	if (not witchMoveX_->GetIsActive()) {
		witchMoveX_->Start(
			false,
			1.5f
		);
	}


	witchMoveX_->Update();

	if (witchMoveX_->ActiveExit()) {

		isFirstEnd_ = true;
	}
	witch_->transform.translate = Vector3::Lerp(Vector3(0.0f, 360.0f, 0.001f), Vector3(0.0f, 200.0f, 0.001f), Easeing::InOutSine(witchMoveX_->GetT()));
	cauldronTransform_->translate = Vector3::Lerp(Vector3::kYIdentity * -360.0f, cauldronBasisPos_, Easeing::InOutSine(witchMoveX_->GetT()));
	cauldronTransform_->scale = cauldronScale_.first;
	witch_->transform.CalcMatrix();
}

void ResultScene::FirstDraw() {
	tex2D_->Draw(
		witch_->transform.matWorld_,
		Mat4x4::MakeScalar(Vector3(0.25f, 1.0f,1.0f)),
		currentCamera_->GetViewOthographics(),
		witchCraftTexID_,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNormal
	);
}

void ResultScene::SecondEffect() {
	curretnActiveFlaskParticleNum_ = 0.0f;
	bool isActive = false;

	if (not flaskParticleAppDurationEase_->GetIsActive()) {
		flaskParticleAppDurationEase_->Start(false, 1.5f, Easeing::OutSine);
	}

	for (auto& i : flaskParticles_) {
		Vector2 appDuration = flaskParticleAppDurationEase_->Get(flaskParticleAppDurationMin, flaskParticleAppDurationMax);
		i->SetFreq(appDuration);
		i->Update();
		if (not isActive) {
			isActive = !!i->GetIsActive();
		}
		curretnActiveFlaskParticleNum_ += static_cast<float>(i->GetCurrentActiveParticleNum());
		flaskParticleAppDurationEase_->Update();
	}
	if (not isActive and not cauldronEase_->GetIsActive()) {
		cauldronEase_->Start(
			false,
			0.3f,
			Easeing::OutBounce
		);
	}

	cauldronEase_->Update();
	if (cauldronEase_->GetIsActive()) {
		cauldronTransform_->scale = cauldronEase_->Get(cauldronScale_.second, cauldronScale_.first);
		cauldronTransform_->translate = cauldronBasisPos_;
	}
	else if (curretnActiveFlaskParticleNum_ != 0.0f) {
		cauldronTransform_->scale = Vector3::Lerp(cauldronScale_.first, cauldronScale_.second, curretnActiveFlaskParticleNum_ / allFlaskParticleNum_);
		cauldronTransform_->translate = cauldronBasisPos_ + Lamb::Random(cauldronShake_.first, cauldronShake_.second);
	}
	else {
		cauldronTransform_->scale = cauldronScale_.first;
		cauldronTransform_->translate = cauldronBasisPos_;
	}

	if (cauldronEase_->ActiveExit()) {
		isFirstActive_ = false;
	}

	if (not witchAnimator_->GetIsActive()) {
		witchAnimator_->Start();
	}
	witchAnimator_->Update();

	witch_->transform.CalcMatrix();

	if(not witchCraft_->IsStart()){
		witchCraft_->Start(0.2f, true);
	}
}
void ResultScene::SecondDraw() {
	for (auto& i : flaskParticles_) {
		i->Draw(currentCamera_->GetViewOthographics());
	}

	tex2D_->Draw(
		witch_->transform.matWorld_,
		witchAnimator_->GetUvMat4x4(),
		currentCamera_->GetViewOthographics(),
		witchCraftTexID_,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNormal
	);
}

void ResultScene::FlyAway() {
	witchMoveX_->Update();
	witchMoveY_->Update();
	witchMoveY2_->Update();

	if (witchMoveY2_->ActiveExit() and not withcEaseingEnd_) {
		withcEaseingEnd_ = true;
	}

	if (witchMoveY_->ActiveExit() and not isStartWitchMoveY2_) {
		witchMoveY2_->Start(
			false,
			0.8f,
			Easeing::OutBounce
		);
		isStartWitchMoveY2_ = true;
	}
	if (not witchMoveY_->GetIsActive() and not witchMoveY2_->GetIsActive() and not isStartWitchMoveY_) {
		witchMoveY_->Start(
			false,
			0.25f,
			Easeing::OutQuad
		);
		isStartWitchMoveY_ = true;
	}
	if (not witchMoveX_->GetIsActive() and not isStartWitchMoveX_) {
		witchMoveX_->Start(
			false,
			0.5f,
			Easeing::OutQuad
		);

		isStartWitchMoveX_ = true;
	}

	witch_->transform.translate.x = witchMoveX_->Get(0.0f, -420.0f);
	if (witchMoveY_->GetIsActive()) {
		witch_->transform.translate.y = witchMoveY_->Get(200.0f, 300.0f);
	}
	else if(witchMoveY2_->GetIsActive()){
		witch_->transform.translate.y = witchMoveY2_->Get(300.0f, -150.0f);
	}
	witch_->transform.CalcMatrix();


	if (withcEaseingEnd_ and not witchResultAnimator_->GetIsActive()) {
		witchResultAnimator_->Start();
	}
	witchResultAnimator_->Update();
}

void ResultScene::WitchDraw() {
	if (witchMoveY2_->GetIsActive()) {
		tex2D_->Draw(
			witch_->transform.matWorld_,
			Mat4x4::kIdentity,
			currentCamera_->GetViewOthographics(),
			witchFlyAwayTexID_,
			std::numeric_limits<uint32_t>::max(),
			BlendType::kNormal
		);
	}
	else if(withcEaseingEnd_ and isGameClear_){
		tex2D_->Draw(
			witch_->transform.matWorld_,
			witchResultAnimator_->GetUvMat4x4(),
			currentCamera_->GetViewOthographics(),
			witchHappyTexID_,
			std::numeric_limits<uint32_t>::max(),
			BlendType::kNormal
		);
	}
	else if (withcEaseingEnd_ and not isGameClear_) {
		tex2D_->Draw(
			witch_->transform.matWorld_,
			witchResultAnimator_->GetUvMat4x4(),
			currentCamera_->GetViewOthographics(),
			witchCryTexID_,
			std::numeric_limits<uint32_t>::max(),
			BlendType::kNormal
		);
	}
}

void ResultScene::GameClearEffect() {
	backGroundEase_->Update();
	backGround_->transform.translate = backGroundEase_->Get(backGroundStartPos_, backGroundEndPos_);
	backGround_->transform.CalcMatrix();
	backGround_->uvTransform.scale.y = 0.5f;
	backGround_->uvTransform.CalcMatrix();
	rotateClearBackFround_ += std::numbers::pi_v<float> * 0.1f* Lamb::DeltaTime();

	if (backGroundEase_->ActiveExit()) {
		resultMessageEase_->Start(
			true,
			0.5f,
			Easeing::InOutSine
		);

		clearItemEase_->Start(
			false,
			0.3f,
			Easeing::OutBounce
		);

		if (not witchGameClearBGM_->IsStart()) {
			witchGameClearBGM_->Start(0.1f, true);
		}
	}

	clearItemEase_->Update();
	clearItem_->transform.translate.y = clearItemEase_->Get(clearItemYPos_.min, clearItemYPos_.max);
	clearItem_->transform.scale = clearItemEase_->Get(clearItemScaleDuration_.first, clearItemScaleDuration_.second);
	clearItem_->transform.CalcMatrix();



	if (not clearItemParticle_->GetIsParticleStart() and clearItemEase_->ActiveExit()) {
		clearItemParticle_->ParticleStart({ clearItem_->transform.translate.x,clearItem_->transform.translate.y,clearItem_->transform.translate.z + 0.001f });
		clearItemParticle_->SetParticleScale(1.0f / 0.0036f);
	}
	clearItemParticle_->Update();

	resultMessageEase_->Update();
	resultMessage_->transform.translate.y = resultMessageEase_->Get(resultMessageYPos_.min, resultMessageYPos_.max);

#ifdef _DEBUG
	ImGui::Begin("ResultUI");
	resultMessage_ ->transform.ImGuiWidget("resultMessageUI");
	ImGui::End();
#endif // _DEBUG

	resultMessage_->transform.CalcMatrix();

	FlyAway();

	UpdateUI();
}

void ResultScene::GameClearDraw() {
	tex2D_->Draw(
		backGround_->transform.matWorld_,
		backGround_->uvTransform.matWorld_ 
		* Mat4x4::MakeTranslate(Vector2::kIdentity * -0.5f) 
		* Mat4x4::MakeScalar(Vector3::kIdentity * 0.7f) * Mat4x4::MakeRotateZ(rotateClearBackFround_)
		* Mat4x4::MakeTranslate(Vector2::kIdentity * 0.5f),
		currentCamera_->GetViewOthographics(),
		backGround_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone
	);
	tex2D_->Draw(
		resultMessage_->transform.matWorld_,
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		resultMessage_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNormal
	);
	tex2D_->Draw(
		clearItem_->transform.matWorld_,
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		clearItem_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNormal
	);
	clearItemParticle_->Draw(
		currentCamera_->rotate,
		currentCamera_->GetViewOthographics()
	);

	WitchDraw();

	DrawUI();
}

void ResultScene::GameOverEffect() {
	backGroundEase_->Update();
	backGround_->transform.translate = backGroundEase_->Get(backGroundStartPos_, backGroundEndPos_);
	backGround_->transform.CalcMatrix();
	backGround_->uvTransform.CalcMatrix();

	if (backGroundEase_->ActiveExit()) {
		resultMessageEase_->Start(
			true,
			0.5f,
			Easeing::InOutSine
		);
		if (not witchGameOverBGM_->IsStart()) {
			witchGameOverBGM_->Start(0.1f, true);
		}
	}

	resultMessageEase_->Update();
	resultMessage_->transform.translate.y = resultMessageEase_->Get(resultMessageYPos_.min, resultMessageYPos_.max);

#ifdef _DEBUG
	ImGui::Begin("ResultUI");
	resultMessage_->transform.ImGuiWidget("resultMessageUI");
	ImGui::End();
#endif // _DEBUG

	resultMessage_->transform.CalcMatrix();

	FlyAway();

	UpdateUI();
}

void ResultScene::GameOverDraw() {
	tex2D_->Draw(
		backGround_->transform.matWorld_,
		backGround_->uvTransform.matWorld_,
		currentCamera_->GetViewOthographics(),
		backGround_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone
	);
	tex2D_->Draw(
		backGround_->transform.matWorld_ * Mat4x4::MakeTranslate(-Vector3::kZIdentity),
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		zanennTexID_,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone
	);

	tex2D_->Draw(
		resultMessage_->transform.matWorld_,
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		resultMessage_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNormal
	);

	WitchDraw();

	DrawUI();
}

void ResultScene::DrawConstantUI() {
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
#ifdef _DEBUG
	UIEditor::GetInstance()->PutDraw(currentTexCamera_->GetViewOthographics());
#endif // _DEBUG
}

void ResultScene::UpdateUI() {
	Lamb::SafePtr gamepad = input_->GetGamepad();
	Lamb::SafePtr key = input_->GetKey();

	if (not isSkip_ and (gamepad->Pushed(Gamepad::Button::A) or key->Pushed(DIK_SPACE))) {
		switch (currentUIPick_)
		{
		case ResultScene::CurrentUIPick::kToNext:
			SelectToGame::GetInstance()->SetSelect(preGameStageNumber_ + 1);
			gameDecision_->Start(0.2f, false);
			sceneManager_->SceneChange(BaseScene::ID::Game);
			break;
		case ResultScene::CurrentUIPick::kRetry:
			SelectToGame::GetInstance()->SetSelect(preGameStageNumber_);
			gameDecision_->Start(0.2f, false);
			sceneManager_->SceneChange(BaseScene::ID::Game);
			break;
		case ResultScene::CurrentUIPick::kToStageSelect:
			gameDecision_->Start(0.2f, false);
			sceneManager_->SceneChange(BaseScene::ID::StageSelect);
			break;
		default:
			break;
		}
	}

	float stick = gamepad->GetStick(Gamepad::Stick::LEFT_Y, 0.3f);
	if (stick == 0.0f and not isStickInput_) {
		isStickInput_ = true;
	}
	if (gamepad->Pushed(Gamepad::Button::UP) or (0.0f < stick and isStickInput_) or key->Pushed(DIK_W) or key->Pushed(DIK_UP)) {
		switch (currentUIPick_)
		{
		case ResultScene::CurrentUIPick::kToNext:
			currentUIPick_ = CurrentUIPick::kToStageSelect;
			break;
		case ResultScene::CurrentUIPick::kRetry:
			currentUIPick_ = CurrentUIPick::kToNext;
			break;
		case ResultScene::CurrentUIPick::kToStageSelect:
			currentUIPick_ = CurrentUIPick::kRetry;
			break;
		default:
			break;
		}
		isStickInput_ = false;
		selectMove_->Start(0.1f, false);
	}
	else if(gamepad->Pushed(Gamepad::Button::DOWN) or (stick < 0.0f and isStickInput_) or key->Pushed(DIK_S) or key->Pushed(DIK_DOWN)) {
		switch (currentUIPick_)
		{
		case ResultScene::CurrentUIPick::kToNext:
			currentUIPick_ = CurrentUIPick::kRetry;
			break;
		case ResultScene::CurrentUIPick::kRetry:
			currentUIPick_ = CurrentUIPick::kToStageSelect;
			break;
		case ResultScene::CurrentUIPick::kToStageSelect:
			currentUIPick_ = CurrentUIPick::kToNext;
			break;
		default:
			break;
		}
		isStickInput_ = false;
		selectMove_->Start(0.1f, false);
	}

	switch (currentUIPick_)
	{
	case ResultScene::CurrentUIPick::kToNext:
		toNextUI_->uvTransform.translate.x = 0.5f;
		retryUI_->uvTransform.translate.x = 0.0f;
		toStageSelectUI_->uvTransform.translate.x = 0.0f;

		toNextUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f } * 1.1f;
		retryUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f };
		toStageSelectUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f };
		break;
	case ResultScene::CurrentUIPick::kRetry:
		toNextUI_->uvTransform.translate.x = 0.0f;
		retryUI_->uvTransform.translate.x = 0.5f;
		toStageSelectUI_->uvTransform.translate.x = 0.0f;

		toNextUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f };
		retryUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f }*1.1f;
		toStageSelectUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f };
		break;
	case ResultScene::CurrentUIPick::kToStageSelect:
		toNextUI_->uvTransform.translate.x = 0.0f;
		retryUI_->uvTransform.translate.x = 0.0f;
		toStageSelectUI_->uvTransform.translate.x = 0.5f;

		toNextUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f };
		retryUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f };
		toStageSelectUI_->transform.scale = Vector3{ 256.0f, 72.0f, 1.0f }*1.1f;
		break;
	default:
		break;
	}

#ifdef _DEBUG
	ImGui::Begin("ResultUI");
	toNextUI_->transform.ImGuiWidget("toNextUI");
	retryUI_->transform.ImGuiWidget("retryUI");
	toStageSelectUI_->transform.ImGuiWidget("toStageSelectUI");
	ImGui::End();
#endif // _DEBUG

	toNextUI_->transform.CalcMatrix();
	toNextUI_->uvTransform.CalcMatrix();
	retryUI_->transform.CalcMatrix();
	retryUI_->uvTransform.CalcMatrix();
	toStageSelectUI_->transform.CalcMatrix();
	toStageSelectUI_->uvTransform.CalcMatrix();
}

void ResultScene::DrawUI() {
	tex2D_->Draw(
		toNextUI_->transform.matWorld_,
		toNextUI_->uvTransform.matWorld_,
		currentCamera_->GetViewOthographics(),
		toNextUI_->textureID,
		toNextUI_->color,
		BlendType::kNormal
	);
	tex2D_->Draw(
		retryUI_->transform.matWorld_,
		retryUI_->uvTransform.matWorld_,
		currentCamera_->GetViewOthographics(),
		retryUI_->textureID,
		retryUI_->color,
		BlendType::kNormal
	);
	tex2D_->Draw(
		toStageSelectUI_->transform.matWorld_,
		toStageSelectUI_->uvTransform.matWorld_,
		currentCamera_->GetViewOthographics(),
		toStageSelectUI_->textureID,
		toStageSelectUI_->color,
		BlendType::kNormal
	);
}

void ResultScene::Skip() {
	Lamb::SafePtr gamepad = input_->GetGamepad();
	Lamb::SafePtr key = input_->GetKey();

	if (not isSkip_ and(gamepad->Pushed(Gamepad::Button::A) or key->Pushed(DIK_SPACE))) {
		witchMoveX_->Stop();

		if (isGameClear_) {
			effectStatus_ = EffectState::kGameClear;
			GameClearEffect();
		}
		// もしゲームオーバーなら
		else {
			effectStatus_ = EffectState::kGameOver;
			GameOverEffect();
		}

		witchCraft_->Stop();

		cauldronParticle_->ParticleStart();
		witchCraftExplotion_->Start(0.2f, false);
		cauldronParticle_->emitterPos.z = currentCamera_->pos.z + 1.0f;

		isSkip_ = true;
	}
}
