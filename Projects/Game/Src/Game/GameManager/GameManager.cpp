#include "GameManager.h"

#include <fstream>

#include "GameObject/Component/EnergyItem.h"
#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include <Game/CollisionManager/Capsule/Capsule.h>
#include <Game/CollisionManager/Collider/Collider.h>
#include <GlobalVariables/GlobalVariables.h>

void GameManager::Init()
{

	input_ = Input::GetInstance();

	blockMap_ = std::make_unique<Map>();
	blockMap_->Init();
}

void GameManager::Update([[maybe_unused]] const float deltaTime)
{
	GlobalVariables::GetInstance()->Update();

	// AABBのデータを転送
	blockMap_->TransferBoxData();
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const
{
	blockMap_->Draw(camera);
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
}