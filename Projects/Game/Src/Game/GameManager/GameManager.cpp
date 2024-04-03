#include "GameManager.h"

#include <fstream>

#include "GameObject/Component/EnergyItem.h"
#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include <Game/CollisionManager/Capsule/Capsule.h>
#include <Game/CollisionManager/Collider/Collider.h>
#include <GlobalVariables/GlobalVariables.h>
#include <GameObject/Component/ModelComp.h>
#include <GameObject/Component/OnBlockMoveComp.h>

void GameManager::Init()
{

	input_ = Input::GetInstance();

	blockMap_ = std::make_unique<Map>();
	blockMap_->Init();

	player_ = std::make_unique<GameObject>();
	{
		auto *const modelComp = player_->AddComponent<ModelComp>();
		modelComp->AddBone("Body", std::make_unique<Model>("Resources/Cube.obj"), { { 0.4f,0.5f,0.4f} });
		modelComp->GetBone("Body")->model_->color = 0xFF0000FF;
	}

	player_->AddComponent<OnBlockMoveComp>();
}

void GameManager::Update([[maybe_unused]] const float deltaTime)
{
	GlobalVariables::GetInstance()->Update();

	player_->Update(deltaTime);

	// AABBのデータを転送
	blockMap_->TransferBoxData();
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const
{
	blockMap_->Draw(camera);
	player_->Draw(camera);
}

bool GameManager::Debug([[maybe_unused]] const char *const str)
{

	bool isChange = false;

#ifdef _DEBUG

	ImGui::Begin(str);

	blockMap_->Debug("BlockMap");

	ImGui::End();

#endif // _DEBUG

	return isChange;
}


void GameManager::InputAction()
{
	// A を押したときに実行
	if (input_->GetGamepad()->Pushed(Gamepad::Button::A)) {

	}

	// プレイヤに付与する移動方向
	Vector3 inputPlayer{};
	// 縦方向の移動
	inputPlayer.z += input_->GetKey()->Pushed(DIK_W);
	inputPlayer.z -= input_->GetKey()->Pushed(DIK_S);

	// 縦方向が無かったら
	if (not inputPlayer.z) {
		// 横方向の移動
		inputPlayer.x -= input_->GetKey()->Pushed(DIK_A);
		inputPlayer.x += input_->GetKey()->Pushed(DIK_D);
	}
	// ベクトルの付与
	player_->GetComponent<OnBlockMoveComp>()->InputMoveDirection(inputPlayer);

}