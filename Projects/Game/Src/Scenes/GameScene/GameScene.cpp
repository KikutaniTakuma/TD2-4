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
#include"Scenes/SelectToGame/SelectToGame.h"
#include "Utils/Random/Random.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::TextureInitialize(){
	/*//一の位
	std::unique_ptr<Tex2DState> dwarfNumTex_;
	//十の位
	std::unique_ptr<Tex2DState> dwarfTenNumTex_;
	//操作UI
	std::unique_ptr<Tex2DState> spaceTex_, keyTex_;*/
	dwarfNumTex_ = std::make_unique<Tex2DState>();
	dwarfNumTex_->transform.scale = { 64.0f,64.0f };
	dwarfNumTex_->transform.translate = { 510.0f, -292.0f ,0 };
	dwarfNumTex_->uvTrnasform.translate = { 0.f, 0.01f };
	dwarfNumTex_->uvTrnasform.scale.x = { 0.1f };
	dwarfNumTex_->color = 0xffffffff;
	dwarfNumTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/dwarfNumber.png");

	dwarfTenNumTex_ = std::make_unique<Tex2DState>();
	dwarfTenNumTex_->transform.scale = { 64.0f,64.0f };
	dwarfTenNumTex_->transform.translate = { 442.0f, -292.0f ,0 };
	dwarfTenNumTex_->uvTrnasform.translate = { 0.f, 0.01f };
	dwarfTenNumTex_->uvTrnasform.scale.x = { 0.1f };
	dwarfTenNumTex_->color = 0xffffffff;
	dwarfTenNumTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/dwarfNumber.png");

	//雲関係
	/*//雲
	std::array<std::unique_ptr<Tex2DState>, 6> clouds_;

	std::array<float, kCloudNum_> cloudsSpeed_;

	std::unordered_map<std::string, Vector2> cloudScale_;

	std::array<std::string, 3> cloudType_;*/

	cloudType_[0] = "cloud1.png";
	cloudType_[1] = "cloud2.png";
	cloudType_[2] = "cloud3.png";

	cloudScale_[cloudType_[0]] = { 480,144 };
	cloudScale_[cloudType_[1]] = { 256,144 };
	cloudScale_[cloudType_[2]] = { 144,96 };

	for (uint32_t i = 0; i < kCloudNum_; i++){
		clouds_[i] = std::make_unique<Tex2DState>();
		uint32_t num = Lamb::Random(0, 2);
		clouds_[i]->textureName = cloudType_[num];
		cloudsSpeed_[i] = Lamb::Random(-3.0f, -1.0f);

		clouds_[i]->transform.translate = { Lamb::Random(0.0f,600.0f), Lamb::Random(-50.0f,250.0f) };
		clouds_[i]->transform.scale = cloudScale_[clouds_[i]->textureName];
		clouds_[i]->color = 0xffffffff;
		clouds_[i]->textureID = DrawerManager::GetInstance()->LoadTexture(("./Resources/UI/GameMain/" + clouds_[i]->textureName).c_str());
	}

	spaceTex_ = std::make_unique<Tex2DState>();
	spaceTex_->transform.scale = { 164.0f,96.0f };
	spaceTex_->transform.translate = { -500.0f, -290.0f ,0 };
	spaceTex_->uvTrnasform.translate = { 0.f, 0.0f };
	spaceTex_->uvTrnasform.scale.x = { 0.5f };
	spaceTex_->color = 0xffffffff;
	spaceTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/makeBlockSpace.png");

	keyTex_ = std::make_unique<Tex2DState>();
	keyTex_->transform.scale = { 128.0f,96.0f };
	keyTex_->transform.translate = { -340.0f, -290.0f ,0 };
	keyTex_->uvTrnasform.translate = { 0.f, 0.00f };
	keyTex_->uvTrnasform.scale.x = { 0.2f };
	keyTex_->color = 0xffffffff;
	keyTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/makeBolckMove.png");

	backGround_ = std::make_unique<Tex2DState>();
	backGround_->transform.scale = { 1280.0f,720.0f };
	backGround_->transform.translate = { 0.0f, 10.0f ,0 };
	backGround_->color = 0xffffffff;
	backGround_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/GameMain/gameBackGround.png");

	
}


void GameScene::Initialize() {
	collisionManager_ = CollisionManager::GetInstance();

	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.y = 10.0f;
	currentCamera_->pos.z = -70.0f;
	currentCamera_->offset.z = -60.0f;
	currentCamera_->offset.y = 8.0f;
	currentCamera_->rotate.x = 0_deg;

	gameManager_ = GameManager::GetInstance();
	gameManager_->Init();

	//aabb_ = AABB::Create({ 0.0f,-0.5f,0.0f }, { 20.0f,1.0f,20.0f });

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
	// player_ = std::make_unique<Player>();
	// player_->Initialize();

	blockEditor_ = std::make_unique<BlockEditor>();
	blockEditor_->Initialize();


	enemyManager_ = EnemyManager::GetInstance();
	enemyManager_->Initialize();

	enemyEditor_ = std::make_unique<EnemyEditor>();
	enemyEditor_->Initialize();

	blockEditor_->LoadFiles(SelectToGame::GetInstance()->GetSelect());
	enemyEditor_->LoadFiles(SelectToGame::GetInstance()->GetSelect());

	enemyMode_ = false;

	TextureInitialize();

	gameBGM_ = audioManager_->Load("./Resources/Sounds/BGM/game.mp3");


	gameBGM_->Start(0.1f, true);
	
}

void GameScene::Finalize() {
	enemyManager_->Finalize();
	// 実体の破棄
	GameManager::Finalize();
}

void GameScene::Update() {
	// デルタタイム
	const float deltaTime = Lamb::DeltaTime();

	currentCamera_->Debug("カメラ");
	
	currentCamera_->Update();

	Debug();

	enemyManager_->Update();
	enemyManager_->Debug();

	gameManager_->InputAction();
	gameManager_->Update(deltaTime);

	/*gameManager_->Debug("GameManager");*/

	collisionManager_->Update();
	collisionManager_->Debug();

	TextureUpdate();

	//player_->AllTrade();

	if (input_->GetKey()->LongPush(DIK_RETURN)&&input_->GetKey()->Pushed(DIK_BACKSPACE)) {
		Audio* cancel = audioManager_->Load("./Resources/Sounds/SE/cancel.mp3");

		gameBGM_->Stop();
		cancel->Start(0.1f, false);

		sceneManager_->SceneChange(BaseScene::ID::StageSelect);
	}
}

void GameScene::TextureUpdate(){
	for (uint32_t i = 0; i < kCloudNum_; i++) {
		clouds_[i]->transform.translate.x += cloudsSpeed_[i];
		if (clouds_[i]->transform.translate.x < (-700.0f) + (-clouds_[i]->transform.scale.x)){
			CloudReset(i);
		}
		clouds_[i]->transform.CalcMatrix();
	}
	dwarfNumTex_->transform.CalcMatrix();
	dwarfNumTex_->uvTrnasform.CalcMatrix();

	dwarfTenNumTex_->transform.CalcMatrix();
	dwarfTenNumTex_->uvTrnasform.CalcMatrix();

	spaceTex_->transform.CalcMatrix();
	spaceTex_->uvTrnasform.CalcMatrix();

	keyTex_->transform.CalcMatrix();
	keyTex_->uvTrnasform.CalcMatrix();

	backGround_->transform.CalcMatrix();

}

void GameScene::CloudReset(const uint32_t cloudNumber){
	clouds_[cloudNumber]->textureName = cloudType_[Lamb::Random(0, 2)];
	clouds_[cloudNumber]->transform.translate = { 640.0f + cloudScale_[clouds_[cloudNumber]->textureName].x,Lamb::Random(-50.0f,250.0f) };
	cloudsSpeed_[cloudNumber] = Lamb::Random(-3.0f, -1.0f);
	clouds_[cloudNumber]->transform.scale = cloudScale_[clouds_[cloudNumber]->textureName];
	clouds_[cloudNumber]->color = 0xffffffff;
	clouds_[cloudNumber]->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/GameMain/" + clouds_[cloudNumber]->textureName);
}


void GameScene::Draw() {
	/*cloud_->Draw();
	skydome_->Draw(*currentCamera_);*/

	// player_->Draw(*currentCamera_);

	enemyManager_->Draw(*currentCamera_);

#ifdef _DEBUG
	tex2D_->Draw(backGround_->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
		, backGround_->textureID, backGround_->color, BlendType::kNormal);

	for (uint32_t i = 0; i < kCloudNum_; i++) {
		tex2D_->Draw(clouds_[i]->transform.matWorld_, Mat4x4::kIdentity, currentCamera_->GetViewOthographics()
			, clouds_[i]->textureID, clouds_[i]->color, BlendType::kNormal);
	}

	UIEditor::GetInstance()->Draw(currentCamera_->GetViewOthographics(), sceneManager_->GetCurrentSceneID());
	if (editorMode_) {
		if (enemyMode_) {
			enemyEditor_->Draw(*currentCamera_);
		}
		else {
			blockEditor_->Draw(*currentCamera_);
		}
	}
	TextureDraw();

	UIEditor::GetInstance()->putDraw(currentCamera_->GetViewOthographics());
#endif // _DEBUG

	gameManager_->Draw(*currentCamera_);

	Lamb::screenout << "Water and cloud scene" << Lamb::endline
		<< "Press space to change ""Model scene""";
}

void GameScene::TextureDraw(){


	

	tex2D_->Draw(dwarfNumTex_->transform.matWorld_, dwarfNumTex_->uvTrnasform.matWorld_, currentCamera_->GetViewOthographics()
		, dwarfNumTex_->textureID, dwarfNumTex_->color, BlendType::kNormal);

	tex2D_->Draw(dwarfTenNumTex_->transform.matWorld_, dwarfTenNumTex_->uvTrnasform.matWorld_, currentCamera_->GetViewOthographics()
		, dwarfTenNumTex_->textureID, dwarfTenNumTex_->color, BlendType::kNormal);

	tex2D_->Draw(spaceTex_->transform.matWorld_, spaceTex_->uvTrnasform.matWorld_, currentCamera_->GetViewOthographics()
		, spaceTex_->textureID, spaceTex_->color, BlendType::kNormal);

	tex2D_->Draw(keyTex_->transform.matWorld_, keyTex_->uvTrnasform.matWorld_, currentCamera_->GetViewOthographics()
		, keyTex_->textureID, keyTex_->color, BlendType::kNormal);
}

void GameScene::Debug(){
#ifdef _DEBUG
	ImGui::Begin("ゲームテクスチャ");
	if (ImGui::TreeNode("一の位")){
		ImGui::DragFloat2("Transform", &dwarfNumTex_->transform.translate.x, 1.0f);
		ImGui::DragFloat2("Scale", &dwarfNumTex_->transform.scale.x, 1.0f);
		ImGui::DragFloat2("UVTransform", &dwarfNumTex_->uvTrnasform.translate.x, 0.01f);
		ImGui::DragFloat2("UVScale", &dwarfNumTex_->uvTrnasform.scale.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("十の位")) {
		ImGui::DragFloat2("Transform", &dwarfTenNumTex_->transform.translate.x, 1.0f);
		ImGui::DragFloat2("Scale", &dwarfTenNumTex_->transform.scale.x, 1.0f);
		ImGui::DragFloat2("UVTransform", &dwarfTenNumTex_->uvTrnasform.translate.x, 0.01f);
		ImGui::DragFloat2("UVScale", &dwarfTenNumTex_->uvTrnasform.scale.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("spaceキー")) {
		ImGui::DragFloat2("Transform", &spaceTex_->transform.translate.x, 1.0f);
		ImGui::DragFloat2("Scale", &spaceTex_->transform.scale.x, 1.0f);
		ImGui::DragFloat2("UVTransform", &spaceTex_->uvTrnasform.translate.x, 0.01f);
		ImGui::DragFloat2("UVScale", &spaceTex_->uvTrnasform.scale.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("WASDキー")) {
		ImGui::DragFloat2("Transform", &keyTex_->transform.translate.x, 1.0f);
		ImGui::DragFloat2("Scale", &keyTex_->transform.scale.x, 1.0f);
		ImGui::DragFloat2("UVTransform", &keyTex_->uvTrnasform.translate.x, 0.01f);
		ImGui::DragFloat2("UVScale", &keyTex_->uvTrnasform.scale.x, 0.01f);
		ImGui::TreePop();
	}
	
	ImGui::End();


	ImGui::Begin("モード変更");
	ImGui::Checkbox("エディターモード", &editorMode_);
	ImGui::Checkbox("エネミーモード", &enemyMode_);
	//ImGui::DragFloat("エディター時のカメラの距離", &editorCameraPosZ_, 1.0f, -100.0f, 0.0f);
	ImGui::End();
	if (input_->GetKey()->Pushed(DIK_E) && input_->GetKey()->LongPush(DIK_LSHIFT)) {
		if (!editorMode_)
			editorMode_ = true;
		else if (editorMode_)
			editorMode_ = false;
	}

	if (editorMode_) {
		currentCamera_->pos.y = -2.0f;
		currentCamera_->pos.z = editorCameraPosZ_;
		if (Mouse::GetInstance()->GetWheelVelocity() != 0) {
			if (!enemyMode_)
				enemyMode_ = true;
			else if (enemyMode_)
				enemyMode_ = false;
		}
		if (enemyMode_) {
			enemyEditor_->MousePosTrans(*currentCamera_);
			enemyEditor_->Debug();
			enemyEditor_->Update();
		}
		else {
			blockEditor_->MousePosTrans(*currentCamera_);
			blockEditor_->Debug();
			blockEditor_->Update();
		}

	}

#endif // _DEBUG
}
