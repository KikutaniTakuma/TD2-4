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

	for (size_t y = 0; y < 3u; y++) {
		// 
		if (ImGui::TreeNode(("階層" + SoLib::to_string(y)).c_str())) {
			for (size_t z = 0; z < 10u; z++) {
				for (size_t x = 0; x < 10u; x++) {
					ImGui::Checkbox(("##Checkbox" + std::to_string(z) + ' ' + std::to_string(x)).c_str(), &reinterpret_cast<bool&>(boxMap_[y][z][x]));
					if (x != 9) {
						ImGui::SameLine();
					}
				}
			}
			ImGui::TreePop();
		}
	}

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

void GameManager::BoxDraw(const Camera &camera) const
{
	// モデルのイテレータ
	std::list<std::unique_ptr<Model>>::const_iterator modelItr = models_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < boxCount_; i++) {
		// モデルを描画
		(*modelItr++)->Draw(camera.GetViewProjection(), camera.GetPos());	// 次のモデルに移動
	}

}

void GameManager::TransferBoxData()
{
	// 箱配列からモデルにデータを渡す

	// 箱の数をリセット
	boxCount_ = 0u;

	// モデルのイテレータ
	std::list<std::unique_ptr<Model>>::iterator modelItr = models_.begin();

	for (size_t y = 0; y < 3u; y++) {
		for (size_t z = 0; z < 10u; z++) {
			for (size_t x = 0; x < 10u; x++) {
				// 箱の状態
				BoxType boxType = static_cast<BoxType>(boxMap_[y][z][x]);

				// 箱が存在するか
				if (boxType != BoxType::kNone) {
					// モデルのイテレータが末尾に到達しているか
					if (modelItr == models_.end()) {
						// 1つ追加する
						models_.push_back(std::make_unique<Model>("Resources/Cube.obj"));
						modelItr = (--models_.end());
					}
					// 現在のモデル
					Model &model = **modelItr;

					// イテレータを加算する
					++modelItr;

					model.pos = Vector3{ x * vBoxDistance_->x, y * vBoxDistance_->y, -(z * vBoxDistance_->x) };
					model.scale = Vector3::kIdentity * 0.5f;
					model.Update();

					boxCount_++;
				}
			}
		}
	}

	//// もしAABBコンテナが末尾に到達していたら
	//if (aabbItr == aabbCollisions_.end()) {
	//	// そこで終了
	//	return;
	//}
	//// AABBコンテナの方が長かった場合は
	//for (; aabbItr != aabbCollisions_.end(); ++aabbItr) {
	//	// モデルを作成する
	//	std::unique_ptr<Model> modelItem = std::make_unique<Model>("Resources/Cube.obj");

	//	// 座標などを代入
	//	modelItem->pos = aabbItr->GetCentor();
	//	modelItem->scale = aabbItr->GetRadius();
	//	modelItem->Update();

	//	// コンテナに保存
	//	models_.push_back(std::move(modelItem));
	//}


}

