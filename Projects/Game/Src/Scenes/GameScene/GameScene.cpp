#include "GameScene.h"
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

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.y = 15.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.25f;
	camera_->offset.z = -60.0f;
	camera_->offset.y = 8.0f;

	water_ = Water::GetInstance();

	cloud_ = Cloud::GetInstance();

	skydome_.reset(new SkyDome);
	skydome_->Initialize();
	skydome_->SetTexture(cloud_->GetTex());

	file_ = std::make_unique<SoLib::IO::File>();
	csv_ = std::make_unique<SoLib::IO::CSV>();
	array2d_ = std::make_unique<SoLib::Array2D<uint32_t>>(0, 0);

	file_->Load("Resources/TestResource/testcsv.csv");
	*file_ >> *csv_;
	array2d_->Resize(csv_->GetHeight(), csv_->GetWidth());
	std::transform(csv_->view().begin(), csv_->view().end(), array2d_->view().begin(), [](const std::string &str)->uint32_t { return std::stoul(str); });
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("カメラ");
	camera_->Update();

	water_->Update(camera_->GetPos());

	cloud_->Update();
	skydome_->Upadate();

	std::string str;
	for (const auto i : array2d_->view()) {
		str += std::to_string(i) + ' ';
	}

	ImGui::Text("%s", str.c_str());

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::START)) {
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}
}

void GameScene::Draw() {
	cloud_->Draw();
	skydome_->Draw(*camera_);

	water_->Draw(camera_->GetViewProjection());

	Lamb::screenout << "Water and cloud scene" << Lamb::endline
		<< "Press space to change ""Model scene""";
}