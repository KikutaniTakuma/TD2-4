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
}

void GameManager::Update([[maybe_unused]] const float deltaTime)
{
	GlobalVariables::GetInstance()->Update();

	// AABBのデータを転送
	TransferBoxData();
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const
{
	BoxDraw(camera);
}

bool GameManager::Debug([[maybe_unused]] const char *const str)
{

	bool isChange = false;

#ifdef _DEBUG

	ImGui::Begin(str);

	for (uint32_t i = 0; auto & aabb : aabbCollisions_) {
		aabb.ImGuiDebug(SoLib::to_string(i++).c_str());
	}

	ImGui::End();

#endif // _DEBUG

	return isChange;
}

void GameManager::AddBox(AABB &&aabb)
{

	aabbCollisions_.push_back(std::move(aabb));

}

void GameManager::InputAction()
{
	// A を押したときに実行
	if (input_->GetGamepad()->Pushed(Gamepad::Button::A)) {

	}
}

void GameManager::BoxDraw(const Camera &camera) const
{
	// モデルのイテレータ
	std::list<std::unique_ptr<Model>>::const_iterator modelItr = models_.begin();

	// aabbの数分のモデルを描画する
	for (size_t i = 0; i < aabbCollisions_.size(); i++) {
		// モデルを描画
		(*modelItr++)->Draw(camera.GetViewProjection(), camera.GetPos());	// 次のモデルに移動
	}

}

void GameManager::TransferBoxData()
{
	// AABBからモデルにデータを渡す

	// 両方のデータの中で、短いほうの長さ
	size_t minSize = std::min(models_.size(), aabbCollisions_.size());
	// そのサイズ分コピー
	// aabbのイテレータ
	std::list<AABB>::iterator aabbItr = aabbCollisions_.begin();
	// モデルのイテレータ
	std::list<std::unique_ptr<Model>>::iterator modelItr = models_.begin();
	for (size_t i = 0; i < minSize; i++) {

		AABB &aabb = *aabbItr;
		Model &model = **modelItr;

		model.pos = aabb.GetCentor();
		model.scale = aabb.GetRadius();
		model.Update();

		++aabbItr;
		++modelItr;
	}

	// もしAABBコンテナが末尾に到達していたら
	if (aabbItr == aabbCollisions_.end()) {
		// そこで終了
		return;
	}
	// AABBコンテナの方が長かった場合は
	for (; aabbItr != aabbCollisions_.end(); ++aabbItr) {
		// モデルを作成する
		std::unique_ptr<Model> modelItem = std::make_unique<Model>("Resources/Cube.obj");

		// 座標などを代入
		modelItem->pos = aabbItr->GetCentor();
		modelItem->scale = aabbItr->GetRadius();
		modelItem->Update();

		// コンテナに保存
		models_.push_back(std::move(modelItem));
	}


}

