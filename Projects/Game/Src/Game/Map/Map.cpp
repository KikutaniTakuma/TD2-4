#include "Map.h"

#include "../SoLib/SoLib/SoLib_Traits.h"
#include <imgui.h>

void Map::Init()
{
	boxMap_ = std::make_unique<MapSize>();
}

void Map::Update([[maybe_unused]] const float deltaTime)
{
}

void Map::Draw(const Camera &camera) const
{
	// モデルのイテレータ
	std::list<std::unique_ptr<Model>>::const_iterator modelItr = models_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < boxCount_; i++) {
		// モデルを描画
		(*modelItr++)->Draw(camera.GetViewProjection(), camera.GetPos());	// 次のモデルに移動
	}

}

bool Map::Debug(const char *const str)
{
	bool isChange = false;

#ifdef _DEBUG

	if (ImGui::TreeNode(str)) {

		for (size_t y = 0; y < 3u; y++) {
			// 
			if (ImGui::TreeNode(("階層" + SoLib::to_string(y)).c_str())) {
				for (size_t z = 0; z < 10u; z++) {
					for (size_t x = 0; x < 10u; x++) {
						isChange |= ImGui::Checkbox(("##Checkbox" + std::to_string(z) + ' ' + std::to_string(x)).c_str(), &reinterpret_cast<bool &>((*boxMap_)[y][z][x]));
						if (x != 9) {
							ImGui::SameLine();
						}
					}
				}
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

#endif // _DEBUG

	return isChange;
}

void Map::TransferBoxData()
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
				BoxType boxType = static_cast<BoxType>((*boxMap_)[y][z][x]);

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

					model.pos = GetGrobalPos(x, y, z);
					model.scale = Vector3::kIdentity * *vBlockScale * 0.5f;
					model.Update();

					boxCount_++;
				}
			}
		}
	}
}
