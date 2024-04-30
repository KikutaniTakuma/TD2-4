#include "GameManager.h"

#include <fstream>

#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include <Game/CollisionManager/Capsule/Capsule.h>
#include <Game/CollisionManager/Collider/Collider.h>
#include <GlobalVariables/GlobalVariables.h>
#include <GameObject/Component/ModelComp.h>
#include "Utils/SafePtr/SafePtr.h"
#include "Drawers/DrawerManager.h"
#include "GameObject/Component/PlayerComp.h"
#include <GameObject/Component/FallingBlockComp.h>
#include <GameObject/Component/Rigidbody.h>

void GameManager::Init()
{

	input_ = Input::GetInstance();

	blockMap_ = std::make_unique<Map>();
	blockMap_->Init();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");

	player_ = std::make_unique<GameObject>();
	{
		Lamb::SafePtr modelComp = player_->AddComponent<ModelComp>();
		modelComp->AddBone("Body", drawerManager->GetModel("Resources/Cube.obj"));
	}

	{
		Lamb::SafePtr playerComp = player_->AddComponent<PlayerComp>();
		// マップの参照を渡す
		playerComp->SetMap(GetMap());
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


void GameManager::AddFallingBlock(Vector2 centorPos, Vector2 size, bool hasDamage, Vector2 velocity, Vector2 gravity)
{
	std::unique_ptr<GameObject> addBlock = std::make_unique<GameObject>();

	// 落下するブロックのコンポーネント
	Lamb::SafePtr fallingComp = addBlock->AddComponent<FallingBlockComp>();

	fallingComp->centerPos_ = centorPos;
	fallingComp->size_ = size;
	fallingComp->hasDamage_ = hasDamage;
	fallingComp->velocity_ = velocity;
	fallingComp->gravity_ = gravity;


	fallingBlocks_.push_back(std::move(addBlock));
}

void GameManager::InputAction()
{
	// A を押したときに実行
	if (input_->GetGamepad()->Pushed(Gamepad::Button::A)) {

	}

	// プレイヤに付与する移動方向
	int32_t inputPlayer{};

	// 横方向の移動
	inputPlayer -= input_->GetKey()->Pushed(DIK_A);
	inputPlayer += input_->GetKey()->Pushed(DIK_D);

	player_->GetComponent<PlayerComp>()->MoveInput(inputPlayer);

	// ベクトルの付与
	//player_->GetComponent<OnBlockMoveComp>()->InputMoveDirection(inputPlayer);

}