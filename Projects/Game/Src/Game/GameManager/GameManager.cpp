#include "GameManager.h"

#include <fstream>

#include "GameObject/Component/EnergyItem.h"
#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include <Game/CollisionManager/Capsule/Capsule.h>
#include <Game/CollisionManager/Collider/Collider.h>
#include <GlobalVariables/GlobalVariables.h>
#include <GameObject/Component/ModelComp.h>

void GameManager::Init()
{

	input_ = Input::GetInstance();

	blockMap_ = std::make_unique<Map>();
	blockMap_->Init();

	player_ = std::make_unique<GameObject>();
	{
		auto *const modelComp = player_->AddComponent<ModelComp>();
		modelComp->AddBone("Body", std::make_unique<Model>("Resources/Cube.obj"));
	}
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

	Vector3 inputPlayer{};
	inputPlayer.z += input_->GetKey()->GetKey(DIK_W);
	inputPlayer.z -= input_->GetKey()->GetKey(DIK_S);

	inputPlayer.x -= input_->GetKey()->GetKey(DIK_A);
	inputPlayer.x += input_->GetKey()->GetKey(DIK_D);

	player_->transform_.translate += inputPlayer;

}