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
	currentCamera_->pos = { 0.f, 0.f ,-10.0f };

	tex2D_ = drawerManager_->GetTexture2D();
	backGround_.translate.z = 50.0f;
	backGround_.scale = Lamb::ClientSize();
	backGroundTextureID_ = drawerManager_->LoadTexture("./Resources/BackGround/gameOverBackGround.png");

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
		i->Resize(20);
		i->SetDeathTime({2.0f, 3.0f});
		i->SetRotate(Vector2(15_deg, 165_deg));
		i->SetRadius(Vector2(300.0f, 400.0f));
		i->SetFreq(Vector2(0.2f, 1.0f));
		//i->SetEndTranslate(Vector3::kYIdentity * -200.0f);
		i->SetTextureID(*texID);
		i->Start();
		texID++;
		if (texID == flaskTextureID_.end()) {
			break;
		}
	}
	backGroundStartPos_ = backGround_.translate;
	backGroundStartPos_.y = Lamb::ClientSize().y;
	backGroundEndPos_ = backGround_.translate;

	backGroundEase_ = std::make_unique<Easeing>();

	isActiveParticle_ = true;
}

void ResultScene::Finalize(){

}

void ResultScene::Update(){
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();

	if (not backGroundEase_->GetIsActive()) {
		isActiveParticle_ = true;
		bool isActive = false;
		for (auto& i : flaskParticles_) {
			i->Update();
			if (i->GetIsActive()) {
				isActive = true;
			}
		}
		isActiveParticle_ = isActive;
	}

	if (isActiveParticle_.OnExit()) {
		backGroundEase_->Start(
			false,
			0.8f,
			Easeing::OutBounce
		);
	}

	backGroundEase_->Update();
	backGround_.translate = backGroundEase_->Get(backGroundStartPos_, backGroundEndPos_);
	backGround_.CalcMatrix();
}

void ResultScene::Draw(){
	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
	tex2D_->Draw(
		backGround_.matWorld_,
		Mat4x4::kIdentity,
		currentCamera_->GetViewOthographics(),
		backGroundTextureID_,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone
	);

	for (auto& i : flaskParticles_) {
		i->Draw(currentCamera_->GetViewOthographics());
	}

#ifdef _DEBUG

	UIEditor::GetInstance()->PutDraw(currentTexCamera_->GetViewOthographics());
#endif // _DEBUG
}

void ResultScene::Debug(){

}
