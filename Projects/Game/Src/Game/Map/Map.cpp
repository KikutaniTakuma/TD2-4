#include "Map.h"

#include "../SoLib/SoLib/SoLib_Traits.h"
#include <imgui.h>
#include"Drawers/DrawerManager.h"

void Map::Init()
{
	boxMap_ = std::make_unique<MapSize>();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");
	

	for (size_t i = 0; i < modelStates_.size(); i++){
		modelStates_[i] = std::make_unique<ModelState>();
		modelStates_[i]->transform.scale = { vBlockScale->x * 0.5f,vBlockScale->x * 0.5f ,vBlockScale->y * 0.5f };
	}
}

void Map::Update([[maybe_unused]] const float deltaTime){

}

void Map::Draw([[maybe_unused]] const Camera& camera) const{
	for (size_t i = 0; i < modelStates_.size(); i++) {
		model_->Draw(modelStates_[i]->transform.matWorld_, camera.GetViewProjection(), modelStates_[i]->color, BlendType::kNormal);
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
						isChange |= ImGui::Checkbox(("##Checkbox" + std::to_string(z) + ' ' + std::to_string(x)).c_str(), &reinterpret_cast<bool&>((*boxMap_)[y][z][x].isMultiSelect_));
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

	for (size_t x = 0; x < kMapX; x++) {
		// 箱の状態
		BoxInfo boxInfo = static_cast<BoxInfo>((*boxMap_)[0][0][x]);
		
		// 現在のモデル
		modelStates_[x]->transform.translate = GetGrobalPos(x, 0, 0);
		modelStates_[x]->transform.CalcMatrix();

		if (boxInfo.isConstruction){
			if (boxInfo.isMultiSelect_){
				modelStates_[x]->color = 0xffff00ff;
			}
			else {
				modelStates_[x]->color = 0x00ff00ff;
			}
		}
		else {
			if (boxInfo.isMultiSelect_) {
				modelStates_[x]->color = 0xff0000ff;
			}
			else {
				modelStates_[x]->color = 0xffffffff;
			}
		}

		
		

		boxCount_++;
	}

}


void Map::MultiReset(){
	for (size_t x = 0; x < kMapX; x++) {
		// 箱の状態
		BoxInfo& boxInfo = ((*boxMap_)[0][0][x]);
		boxInfo.isMultiSelect_ = false;
	}
}

const Map::BoxInfo Map::GetBoxInfo(const Vector3& localPos) const{
	// もしマップ外に行っていた場合虚無
	if (localPos.x < 0.f or localPos.y < 0.f or localPos.z < 0.f or localPos.x >= Map::kMapX or localPos.y >= Map::kMapY or localPos.z >= Map::kMapZ) {
		return BoxInfo();
	}

	return BoxInfo();
}

const Map::BoxType Map::GetBoxType(const Vector3& localPos) const{
	// もしマップ外に行っていた場合虚無
	if (localPos.x < 0.f or localPos.y < 0.f or localPos.z < 0.f or localPos.x >= Map::kMapX or localPos.y >= Map::kMapY or localPos.z >= Map::kMapZ) {
		return BoxType();
	}


	return BoxType();
}
