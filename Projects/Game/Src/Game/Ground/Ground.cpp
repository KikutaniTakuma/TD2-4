#include "Ground.h"
#include "../SoLib/SoLib/SoLib_Traits.h"
#include <imgui.h>
#include"Drawers/DrawerManager.h"
#include <Game/Map/Map.h>

void Ground::Init(){
	boxMap_ = std::make_unique<MapSize>();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");


	for (size_t i = 0; i < modelStates_.size(); i++) {
		modelStates_[i] = std::make_unique<ModelState>();
		modelStates_[i]->transform.scale = { vBlockScale->x * 0.5f,vBlockScale->x * 0.5f ,vBlockScale->y * 0.5f };
	}
}

void Ground::Update([[maybe_unused]] const float deltaTime){

}

void Ground::Draw([[maybe_unused]] const Camera& camera) const {
	for (size_t i = 0; i < modelStates_.size(); i++) {
		model_->Draw(modelStates_[i]->transform.matWorld_, camera.GetViewProjection(), modelStates_[i]->color, BlendType::kNormal);
	}
}

//bool Ground::Debug(const char* const str){
//
//	return false;
//}

void Ground::TransferBoxData(){
	for (size_t x = 0; x < kMapX; x++) {
		// 箱の状態
		GroundInfo boxInfo = static_cast<GroundInfo>((*boxMap_)[x]);

		// 現在のモデル
		modelStates_[x]->transform.translate = GetGrobalPos(x, 0, 0);
		modelStates_[x]->transform.CalcMatrix();

		if (boxInfo.isConstruction) {
			if (boxInfo.isMultiSelect_) {
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
		
	}
}

void Ground::MultiReset(){
	for (size_t x = 0; x < kMapX; x++) {
		// 箱の状態
		GroundInfo& boxInfo = ((*boxMap_)[x]);
		boxInfo.isMultiSelect_ = false;
	}
}

const Ground::GroundInfo Ground::GetGroundInfo(const Vector3& localPos) const{

	// もしマップ外に行っていた場合虚無
	if (localPos.x < 0.f or localPos.y < 0.f or localPos.z < 0.f or localPos.x >= Map::kMapX or localPos.y >= 1 or localPos.z >= 1) {
		return GroundInfo();
	}

	return GroundInfo();
}
