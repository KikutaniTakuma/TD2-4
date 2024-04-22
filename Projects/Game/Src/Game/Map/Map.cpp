#include "Map.h"

#include "../SoLib/SoLib/SoLib_Traits.h"
#include <imgui.h>
#include"Drawers/DrawerManager.h"

void Map::Init()
{
	boxMap_ = std::make_unique<MapSize>();
	houseList_.clear();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");


	for (size_t i = 0; i < modelStates_.size(); i++) {
		modelStates_[i] = std::make_unique<ModelState>();
		modelStates_[i]->transform.scale = { vBlockScale->x * 0.5f,vBlockScale->x * 0.5f ,vBlockScale->y * 0.5f };
	}
}

void Map::Update([[maybe_unused]] const float deltaTime) {

}

void Map::Draw([[maybe_unused]] const Camera &camera) const {
	for (size_t i = 0; i < modelStates_.size(); i++) {
		model_->Draw(modelStates_[i]->transform.matWorld_, camera.GetViewProjection(), modelStates_[i]->color, BlendType::kNormal);
	}
}

bool Map::Debug(const char *const str)
{
	bool isChange = false;

#ifdef _DEBUG

	if (ImGui::TreeNode(str)) {

		for (size_t y = 0; y < kMapY; y++) {
			// 
			//if (ImGui::TreeNode(("階層" + SoLib::to_string(y)).c_str())) {
				//for (size_t z = 0; z < kMapZ; z++) {
			for (size_t x = 0; x < kMapX; x++) {
				isChange |= ImGui::Checkbox(("##Checkbox" + std::to_string(y) + ' ' + std::to_string(x)).c_str(), &reinterpret_cast<bool &>((*boxMap_)[y][x]));
				if (x != 9) {
					ImGui::SameLine();
				}
			}
			//}
			//ImGui::TreePop();
		//}
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
	//boxCount_ = 0u;


	//	boxCount_++;

	// 拠点のデータの転送
	for (auto &house : houseList_) {

		// 現在のモデル
		house.houseModelState_.transform.translate = GetGrobalPos(house.xPos_, 0);
		house.houseModelState_.transform.CalcMatrix();

		/*if (house.isConstruction) {
			if (house.isMultiSelect_) {
				house.houseModelState_.color = 0xffff00ff;
			}
			else {
				house.houseModelState_.color = 0x00ff00ff;
			}
		}
		else {
			if (house.isMultiSelect_) {
				house.houseModelState_.color = 0xff0000ff;
			}
			else {
				house.houseModelState_.color = 0xffffffff;
			}
		}*/



	}

}


void Map::MultiReset() {
	for (auto &house : houseList_) {
		house.isMultiSelect_ = false;
	}
}

//const Map::HouseInfo &Map::GetHouseInfo(const int localPosX) const {
//	// もしマップ外に行っていた場合虚無
//	if (IsOutSide({ localPosX,0 })) {
//		return HouseInfo{};
//	}
//
//	return BoxType();
//}

const Map::BoxType Map::GetBoxType(const Vector2 &localPos) const {
	// もしマップ外に行っていた場合虚無
	if (IsOutSide(localPos)) {
		return BoxType::kNone;
	}

	// ブロックのデータを返す
	return (*boxMap_)[int(localPos.y)][int(localPos.x)];
}

bool Map::IsOutSide(const Vector2 &localPos) const
{
	return localPos.x < 0.f or localPos.y < 0.f or localPos.x >= Map::kMapX or localPos.y >= Map::kMapY;
}
