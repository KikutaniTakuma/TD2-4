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

ResultScene::ResultScene():
BaseScene{ BaseScene::ID::Result }
{
}

void ResultScene::Initialize(){
	effectStatus_ = EffectState::kFirst;

	currentCamera_->pos = { 0.f, 0.f ,-10.0f };

	tex2D_ = drawerManager_->GetTexture2D();
	backGround_ = std::make_unique<Transform>();
	backGround_->translate.z = 50.0f;
	backGround_->scale = Lamb::ClientSize();
	isGameClear_ = false;
	if (isGameClear_) {
		backGroundTextureID_ = drawerManager_->LoadTexture("./Resources/BackGround/gameOverBackGround.png");
		currentUIPick_ = CurrentUIPick::kToNext;
	}
	else {
		backGroundTextureID_ = drawerManager_->LoadTexture("./Resources/BackGround/gameOverBackGround.png");
		currentUIPick_ = CurrentUIPick::kRetry;
	}
		zanennTexID_ = drawerManager_->LoadTexture("./Resources/Result/gameOverEffect.png");
	//clearTextureID_ = drawerManager_->LoadTexture("./Resources/BackGround/gameOverBackGround.png");

	cauldronParticle_ = std::make_unique<Particle>();
	cauldronParticle_->LoadSettingDirectory("Bomb");
	cauldronTextureID_ = drawerManager_->LoadTexture("./Resources/UI/pot.png");
	cauldronTransform_ = std::make_unique<Transform>();
	cauldronBasisPos_ = Vector3::kYIdentity * -128.0f;
	cauldronShake_.first = (Vector3::kXIdentity + Vector3::kYIdentity) * -20.0f;
	cauldronShake_.second = (Vector3::kXIdentity + Vector3::kYIdentity) * 20.0f;
	cauldronScale_.first = (Vector3::kXIdentity + Vector3::kYIdentity) * 450.0f;
	cauldronScale_.second = cauldronScale_.first * 1.5f;
	cauldronEase_ = std::make_unique<Easeing>();


	for (size_t index = 0; const auto& i : std::filesystem::directory_iterator("./Resources/Item/")) {
		if (not i.path().has_extension()) {
			continue;
		}
		flaskTextureID_[index] = drawerManager_->LoadTexture(i.path().string());
		index++;
		if (flaskTextureID_.size() <= index) {
			break;
		}
	}

	for (auto texID = flaskTextureID_.begin(); auto & i : flaskParticles_) {
		i = std::make_unique<FlaskParticle>();
		i->SetParticleSize(Vector3::kIdentity * 50.0f, Vector3::kIdentity * 80.0f);

		// ここでゲームプレイ中のデータを入れる予定
		i->Resize(20);
		allFlaskParticleNum_ += static_cast<float>(i->GetSize());


		i->SetDeathTime({1.0f, 1.5f});
		i->SetRotate(Vector2(15_deg, 165_deg));
		i->SetRadius(Vector2(300.0f, 400.0f));
		i->SetFreq(Vector2(0.3f, 0.8f));
		i->SetEndTranslate(Vector3::kYIdentity * 60.0f);
		i->SetTextureID(*texID);
		i->Start();
		texID++;
		if (texID == flaskTextureID_.end()) {
			break;
		}
	}
	backGroundStartPos_ = backGround_->translate;
	backGroundStartPos_.y = Lamb::ClientSize().y;
	backGroundEndPos_ = backGround_->translate;

	backGroundEase_ = std::make_unique<Easeing>();

	isFirstActive_ = true;

	toNextUI_  = std::make_unique<Tex2DState>();
	retryUI_ = std::make_unique<Tex2DState>();
	toStageSelectUI_ = std::make_unique<Tex2DState>();

	toNextUI_->textureID = drawerManager_->LoadTexture("./Resources/Result/next.png");
	retryUI_->textureID = drawerManager_->LoadTexture("./Resources/Result/retry.png");
	toStageSelectUI_->textureID = drawerManager_->LoadTexture("./Resources/Result/titleSelect.png");

	toNextUI_->uvTransform.scale.x = 0.5f;
	retryUI_->uvTransform.scale.x = 0.5f;
	toStageSelectUI_->uvTransform.scale.x = 0.5f;

}

void ResultScene::Finalize(){

}

void ResultScene::Update(){
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();


	switch (effectStatus_)
	{
	case ResultScene::EffectState::kFirst:
		FirstEffect();
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


	if (isFirstActive_.OnExit()) {
		//if(もしゲームクリアなら){
		//effectStatus_ = EffectState::kGameClear;
		//}
		// もしゲームオーバーなら
		//else{
		effectStatus_ = EffectState::kGameOver;
		//}

		cauldronParticle_->ParticleStart();
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
	curretnActiveFlaskParticleNum_ = 0.0f;
	bool isActive = false;
	for (auto& i : flaskParticles_) {
		i->Update();
		if (not isActive) {
			isActive = !!i->GetIsActive();
		}
		curretnActiveFlaskParticleNum_ += static_cast<float>(i->GetCurrentActiveParticleNum());
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
}
void ResultScene::FirstDraw() {
	for (auto& i : flaskParticles_) {
		i->Draw(currentCamera_->GetViewOthographics());
	}
}

void ResultScene::GameClearEffect() {
	backGroundEase_->Update();
	backGround_->translate = backGroundEase_->Get(backGroundStartPos_, backGroundEndPos_);
	backGround_->CalcMatrix();

	UpdateUI();
}

void ResultScene::GameClearDraw() {
	tex2D_->Draw(
		backGround_->matWorld_,
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		backGroundTextureID_,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone
	);

	DrawUI();
}

void ResultScene::GameOverEffect() {
	backGroundEase_->Update();
	backGround_->translate = backGroundEase_->Get(backGroundStartPos_, backGroundEndPos_);
	backGround_->CalcMatrix();

	UpdateUI();
}

void ResultScene::GameOverDraw() {
	tex2D_->Draw(
		backGround_->matWorld_,
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		backGroundTextureID_,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone
	);
	tex2D_->Draw(
		backGround_->matWorld_ * Mat4x4::MakeTranslate(-Vector3::kZIdentity),
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		zanennTexID_,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone
	);

	DrawUI();
}

void ResultScene::DrawConstantUI() {
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
#ifdef _DEBUG
	UIEditor::GetInstance()->PutDraw(currentTexCamera_->GetViewOthographics());
#endif // _DEBUG
}

void ResultScene::UpdateUI() {
	switch (currentUIPick_)
	{
	case ResultScene::CurrentUIPick::kToNext:
		toNextUI_->uvTransform.translate.x = 0.5f;
		retryUI_->uvTransform.translate.x = 0.0f;
		toStageSelectUI_->uvTransform.translate.x = 0.0f;
		break;
	case ResultScene::CurrentUIPick::kRetry:
		toNextUI_->uvTransform.translate.x = 0.0f;
		retryUI_->uvTransform.translate.x = 0.5f;
		toStageSelectUI_->uvTransform.translate.x = 0.0f;
		break;
	case ResultScene::CurrentUIPick::kToStageSelect:
		toNextUI_->uvTransform.translate.x = 0.0f;
		retryUI_->uvTransform.translate.x = 0.0f;
		toStageSelectUI_->uvTransform.translate.x = 0.5f;
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
