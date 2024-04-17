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

void Map::Draw(const Camera& camera) const
{
	// モデルのイテレータ
	std::list<std::unique_ptr<Model>>::const_iterator modelItr = models_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < kMapX; i++) {
		// モデルを描画
		(*modelItr++)->Draw(camera.GetViewProjection(), camera.GetPos());	// 次のモデルに移動
	}

}

bool Map::Debug(const char* const str)
{
	bool isChange = false;

#ifdef _DEBUG

	if (ImGui::TreeNode(str)) {

		for (size_t y = 0; y < kMapY; y++) {
			// 
			if (ImGui::TreeNode(("階層" + SoLib::to_string(y)).c_str())) {
				for (size_t z = 0; z < kMapZ; z++) {
					for (size_t x = 0; x < kMapX; x++) {
						isChange |= ImGui::Checkbox(("##Checkbox" + std::to_string(z) + ' ' + std::to_string(x)).c_str(), &reinterpret_cast<bool&>((*boxMap_)[y][z][x]));
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

	for (size_t y = 0; y < kMapY; y++) {
		// 描画フラグが有効であった場合は描画
		if (isFloorDrawing_[y]) {

			for (size_t z = 0; z < kMapZ; z++) {
				for (size_t x = 0; x < kMapX; x++) {
					// 箱の状態
					BoxInfo boxInfo = static_cast<BoxInfo>((*boxMap_)[y][z][x]);

					// モデルのイテレータが末尾に到達しているか
					if (modelItr == models_.end()) {
						// 1つ追加する
						models_.push_back(std::make_unique<Model>("Resources/Cube.obj"));
						modelItr = (--models_.end());
					}
					// 現在のモデル
					Model& model = **modelItr;

					// イテレータを加算する
					++modelItr;

					model.pos = GetGrobalPos(x, y, z);
					model.scale.x = vBlockScale->x * 0.5f;
					model.scale.y = vBlockScale->x * 0.5f;
					model.scale.z = vBlockScale->y * 0.5f;
					model.Update();
					if (boxInfo.isConstruction == true) {
						if (boxInfo.isMultiSelect_){
							model.color = 0xffff00ff;
						}
						else {
							model.color = 0x00ff00ff;
						}
					}
					else {
						if (boxInfo.isMultiSelect_) {
							model.color = 0xff0000ff;
						}
						else {
							model.color = 0xffffffff;
						}
					}

					boxCount_++;
					
				}
			}

		}
	}
}

void Map::MultiReset(){
	for (size_t y = 0; y < kMapY; y++) {

		for (size_t z = 0; z < kMapZ; z++) {
			for (size_t x = 0; x < kMapX; x++) {
				// 箱の状態
				BoxInfo& boxInfo = ((*boxMap_)[y][z][x]);
				boxInfo.isMultiSelect_ = false;
			}
		}

		
	}

}

const Map::BoxInfo Map::GetBoxInfo(const Vector3 &localPos) const
{

	// もしマップ外に行っていた場合虚無
	if (localPos.x < 0.f or localPos.y < 0.f or localPos.z < 0.f or localPos.x >= Map::kMapX or localPos.y >= Map::kMapY or localPos.z >= Map::kMapZ) {
		return BoxInfo();
	}


	return (*boxMap_)[static_cast<uint32_t>(localPos.y)][static_cast<uint32_t>(localPos.z)][static_cast<uint32_t>(localPos.x)];

}

const Map::BoxType Map::GetBoxType(const Vector3& localPos) const{
	// もしマップ外に行っていた場合虚無
	if (localPos.x < 0.f or localPos.y < 0.f or localPos.z < 0.f or localPos.x >= Map::kMapX or localPos.y >= Map::kMapY or localPos.z >= Map::kMapZ) {
		return BoxType();
	}


	return BoxType();
}
