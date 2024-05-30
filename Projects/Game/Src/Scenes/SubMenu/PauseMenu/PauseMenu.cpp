#include "PauseMenu.h"
#include "AudioManager/AudioManager.h"

#include "Drawers/DrawerManager.h"
#include "Input/Input.h"

#include "Utils/EngineInfo/EngineInfo.h"

#include "Scenes/Manager/SceneManager.h"
#include "Scenes/SelectToGame/SelectToGame.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

#include <climits>

void PauseMenu::Initialize() {
	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	tex2D_ = drawerManager->GetTexture2D();

	usuGurai_ = std::make_unique<Tex2DState>();
	usuGurai_->textureID = TextureManager::GetInstance()->GetWhiteTex();
	usuGurai_->transform.scale = Lamb::ClientSize();
	menu_ = std::make_unique<Tex2DState>();
	menu_->transform.scale = { 800.0f, 300.0f };
	menu_->textureID = drawerManager->LoadTexture("./Resources/Pause/pauseFrame.png");
	modoru_ = std::make_unique<Tex2DState>();
	modoru_->transform.scale = { 100.0f, 60.0f };
	modoru_->transform.translate = {-230.0f, 100.0f };
	modoru_->textureID = drawerManager->LoadTexture("./Resources/Pause/back.png");
	modoruUI_ = std::make_unique<Tex2DState>();
	modoruUI_->textureID = drawerManager->LoadTexture("./Resources/Pause/pauseButton.png");
	modoruUI_->uvTransform.scale.x = 0.5f;
	modoruUI_->transform.scale = { 60.0f, 60.0f };
	modoruUI_->transform.translate = { -315.0f, 100.0f };
	retry_ = std::make_unique<Tex2DState>();
	retry_->uvTransform.scale.x = 0.5f;
	retry_->textureID = drawerManager->LoadTexture("./Resources/Result/retry.png");
	retry_->transform.scale = { 350.0f, 120.0f };
	retry_->transform.translate = { -190.0f, -30.0f };
	stageSelect_ = std::make_unique<Tex2DState>();
	stageSelect_->uvTransform.scale.x = 0.5f;
	stageSelect_->textureID = drawerManager->LoadTexture("./Resources/Result/stageSelect.png");
	stageSelect_->transform.scale = { 350.0f, 120.0f };
	stageSelect_->transform.translate = { 190.0f, -30.0f };
	puaseMenuUI_ = std::make_unique<Tex2DState>();
	puaseMenuUI_->textureID = drawerManager->LoadTexture("./Resources/Pause/pause.png");
	puaseMenuUI_->transform.scale = { 200.0f, 60.0f };
	puaseMenuUI_->transform.translate = { 0.0f, 100.0f };

	isRetryChoice_ = true;
	isStick_ = false;

	Lamb::SafePtr audioManager = AudioManager::GetInstance();

	modoruSE_ = audioManager->Load("./Resources/Sounds/SE/cancel.mp3");
	selectSE_ = audioManager->Load("./Resources/Sounds/SE/selectMove.mp3");
	choiceSE_ = audioManager->Load("./Resources/Sounds/SE/choice.mp3");

	camera_->pos.z = -10.0f;
	camera_->Update();
}

void PauseMenu::Finalize() {
}

void PauseMenu::Draw() {
	tex2D_->Draw(
		usuGurai_->transform.matWorld_,
		usuGurai_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		usuGurai_->textureID,
		0xaf,
		BlendType::kUnenableDepthNormal
	);
	tex2D_->Draw(
		menu_->transform.matWorld_,
		menu_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		menu_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
	tex2D_->Draw(
		puaseMenuUI_->transform.matWorld_,
		Mat4x4::kIdentity,
		camera_->GetViewOthographics(),
		puaseMenuUI_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
	tex2D_->Draw(
		modoru_->transform.matWorld_,
		modoru_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		modoru_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
	tex2D_->Draw(
		modoruUI_->transform.matWorld_,
		modoruUI_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		modoruUI_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
	tex2D_->Draw(
		retry_->transform.matWorld_,
		retry_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		retry_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
	tex2D_->Draw(
		stageSelect_->transform.matWorld_,
		stageSelect_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		stageSelect_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
}

void PauseMenu::Update() {
	Lamb::SafePtr gamepad = Input::GetInstance()->GetGamepad();
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	float stick = gamepad->GetStick(Gamepad::Stick::LEFT_X);
	if (stick == 0.0f) {
		isStick_ = false;
	}

	if ((gamepad->Pushed(Gamepad::Button::START) or key->Pushed(DIK_ESCAPE)) and not isActive_.OnEnter()) {
		isActive_ = false;
		modoruSE_->Start(0.2f, false);
	}
	if ((stick != 0.0f and not isStick_) or gamepad->Pushed(Gamepad::Button::RIGHT) or key->Pushed(DIK_RIGHT) or key->Pushed(DIK_D)
		or gamepad->Pushed(Gamepad::Button::LEFT) or key->Pushed(DIK_LEFT) or key->Pushed(DIK_A)) {
		isRetryChoice_ = !isRetryChoice_;
		selectSE_->Start(0.1f, false);
		isStick_ = true;
	}
	if (gamepad->Pushed(Gamepad::Button::A) or key->Pushed(DIK_SPACE)) {
		choiceSE_->Start(0.2f, false);
		if (isRetryChoice_) {
			sceneManager_->SceneChange(BaseScene::ID::Game);
			SelectToGame::GetInstance()->SetRetryFlug(true);
		}
		else {
			sceneManager_->SceneChange(BaseScene::ID::StageSelect);
		}
	}

#ifdef _DEBUG
	ImGui::Begin("PauseMenu");
	usuGurai_->transform.ImGuiWidget("usuGurai");
	menu_->transform.ImGuiWidget("menu");
	modoru_->transform.ImGuiWidget("modoru");
	modoruUI_->transform.ImGuiWidget("modoruUI");
	retry_->transform.ImGuiWidget("retry");
	stageSelect_->transform.ImGuiWidget("stageSelect");
	puaseMenuUI_->transform.ImGuiWidget("puaseMenuUI_");
	ImGui::End();
#endif // _DEBUG

	if (isRetryChoice_) {
		retry_->uvTransform.translate.x = 0.5f;
		stageSelect_->uvTransform.translate.x = 0.0f;
	}
	else {
		retry_->uvTransform.translate.x = 0.0f;
		stageSelect_->uvTransform.translate.x = 0.5f;
	}
	if (not isActive_) {
		modoruUI_->uvTransform.translate.x = 0.5f;
	}

	retry_->uvTransform.CalcMatrix();
	stageSelect_->uvTransform.CalcMatrix();
	modoruUI_->uvTransform.CalcMatrix();
	usuGurai_->transform.CalcMatrix();
	menu_->transform.CalcMatrix();
	modoru_->transform.CalcMatrix();
	modoruUI_->transform.CalcMatrix();
	retry_->transform.CalcMatrix();
	stageSelect_->transform.CalcMatrix();
	puaseMenuUI_->transform.CalcMatrix();
}
