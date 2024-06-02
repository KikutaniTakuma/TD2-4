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
	menu_->transform.scale = { 400.0f, 450.0f };
	menu_->transform.translate.y = 10.0f;
	menu_->textureID = drawerManager->LoadTexture("./Resources/Pause/pauseFrame.png");
	modoruUI_ = std::make_unique<Tex2DState>();
	modoruUI_->textureID = drawerManager->LoadTexture("./Resources/Pause/gameBack.png");
	modoruUI_->uvTransform.scale.x = 0.5f;
	modoruUI_->transform.scale = { 350.0f, 120.0f };
	modoruUI_->transform.translate = { 0.0f, 90.0f };
	retryUI_ = std::make_unique<Tex2DState>();
	retryUI_->uvTransform.scale.x = 0.5f;
	retryUI_->textureID = drawerManager->LoadTexture("./Resources/Result/retry.png");
	retryUI_->transform.scale = { 350.0f, 120.0f };
	retryUI_->transform.translate = { 0.0f, -20.0f };
	stageSelectUI_ = std::make_unique<Tex2DState>();
	stageSelectUI_->uvTransform.scale.x = 0.5f;
	stageSelectUI_->textureID = drawerManager->LoadTexture("./Resources/Result/stageSelect.png");
	stageSelectUI_->transform.scale = { 350.0f, 120.0f };
	stageSelectUI_->transform.translate = { 0.0f, -130.0f };
	puaseMenuUI_ = std::make_unique<Tex2DState>();
	puaseMenuUI_->textureID = drawerManager->LoadTexture("./Resources/Pause/pause.png");
	puaseMenuUI_->transform.scale = { 180.0f, 60.0f };
	puaseMenuUI_->transform.translate = { 0.0f, 195.0f, -1.0f };

	curretnState_ = State::kBack;

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
		modoruUI_->transform.matWorld_,
		modoruUI_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		modoruUI_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
	tex2D_->Draw(
		retryUI_->transform.matWorld_,
		retryUI_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		retryUI_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
	tex2D_->Draw(
		stageSelectUI_->transform.matWorld_,
		stageSelectUI_->uvTransform.matWorld_,
		camera_->GetViewOthographics(),
		stageSelectUI_->textureID,
		std::numeric_limits<uint32_t>::max(),
		BlendType::kUnenableDepthNormal
	);
}

void PauseMenu::Update() {

#ifdef _DEBUG
	ImGui::Begin("PauseMenu");
	usuGurai_->transform.ImGuiWidget("usuGurai");
	menu_->transform.ImGuiWidget("menu");
	modoruUI_->transform.ImGuiWidget("modoruUI");
	retryUI_->transform.ImGuiWidget("retry");
	stageSelectUI_->transform.ImGuiWidget("stageSelect");
	puaseMenuUI_->transform.ImGuiWidget("puaseMenuUI_");
	ImGui::End();
#endif // _DEBUG

	ChangeState();

	StateUpdate();

	SceneChange();

	retryUI_->uvTransform.CalcMatrix();
	stageSelectUI_->uvTransform.CalcMatrix();
	modoruUI_->uvTransform.CalcMatrix();
	usuGurai_->transform.CalcMatrix();
	menu_->transform.CalcMatrix();
	modoruUI_->transform.CalcMatrix();
	retryUI_->transform.CalcMatrix();
	stageSelectUI_->transform.CalcMatrix();
	puaseMenuUI_->transform.CalcMatrix();
}

void PauseMenu::ChangeUP() {
	switch (curretnState_)
	{
	case PauseMenu::State::kBack:
		curretnState_ = State::kStatgeSelect;
		break;
	case PauseMenu::State::kRetry:
		curretnState_ = State::kBack;
		break;
	case PauseMenu::State::kStatgeSelect:
		curretnState_ = State::kRetry;
		break;
	default:
		break;
	}
}

void PauseMenu::ChangeDown()
{
	switch (curretnState_)
	{
	case PauseMenu::State::kBack:
		curretnState_ = State::kRetry;
		break;
	case PauseMenu::State::kRetry:
		curretnState_ = State::kStatgeSelect;
		break;
	case PauseMenu::State::kStatgeSelect:
		curretnState_ = State::kBack;
		break;
	default:
		break;
	}
}

void PauseMenu::ChangeState()
{
	Lamb::SafePtr gamepad = Input::GetInstance()->GetGamepad();
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	float stick = gamepad->GetStick(Gamepad::Stick::LEFT_Y);
	if (stick == 0.0f) {
		isStick_ = false;
	}
	if ((0.0f < stick and not isStick_)
		or gamepad->Pushed(Gamepad::Button::UP) 
		or key->Pushed(DIK_UP) 
		or key->Pushed(DIK_W)) 
	{
		selectSE_->Start(0.1f, false);
		isStick_ = true;
		ChangeUP();
	}
	else if (
		(stick < 0.0f and not isStick_)
		or gamepad->Pushed(Gamepad::Button::DOWN) 
		or key->Pushed(DIK_DOWN) 
		or key->Pushed(DIK_S))
	{
		selectSE_->Start(0.1f, false);
		isStick_ = true;
		ChangeDown();
	}
}

void PauseMenu::SceneChange() {
	Lamb::SafePtr gamepad = Input::GetInstance()->GetGamepad();
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	if (gamepad->Pushed(Gamepad::Button::B)) {
		curretnState_ = State::kBack;
		isActive_ = false;
	}
	if (gamepad->Pushed(Gamepad::Button::A) or key->Pushed(DIK_SPACE)) {
		switch (curretnState_)
		{
		case PauseMenu::State::kBack:
			isActive_ = false;
			break;
		case PauseMenu::State::kRetry:
			sceneManager_->SceneChange(BaseScene::ID::Game);
			break;
		case PauseMenu::State::kStatgeSelect:
			sceneManager_->SceneChange(BaseScene::ID::StageSelect);
			break;
		default:
			break;
		}
	}
}

void PauseMenu::StateUpdate() {
	switch (curretnState_)
	{
	case PauseMenu::State::kBack:
		modoruUI_->uvTransform.translate.x = 0.5f;
		retryUI_->uvTransform.translate.x = 0.0f;
		stageSelectUI_->uvTransform.translate.x = 0.0f;
		break;
	case PauseMenu::State::kRetry:
		modoruUI_->uvTransform.translate.x = 0.0f;
		retryUI_->uvTransform.translate.x = 0.5f;
		stageSelectUI_->uvTransform.translate.x = 0.0f;
		break;
	case PauseMenu::State::kStatgeSelect:
		modoruUI_->uvTransform.translate.x = 0.0f;
		retryUI_->uvTransform.translate.x = 0.0f;
		stageSelectUI_->uvTransform.translate.x = 0.5f;
		break;
	default:
		break;
	}
}
