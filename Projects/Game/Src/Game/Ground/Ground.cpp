#include "Ground.h"
#include "../SoLib/SoLib/SoLib_Traits.h"
#include <imgui.h>
#include"Drawers/DrawerManager.h"
#include <Game/Map/Map.h>
#include <SoLib/Math/Math.hpp>
#include "Game/GameManager/GameManager.h"

void Ground::Init()
{
	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");

	// ステージの中心に設置
	groundModelStates_.transMat = SoLib::Math::Affine(Vector3{ static_cast<float>(Map::kMapX) / 2.f ,0.5f,5.f }, Vector3::kZero, Map::GetGrobalPos(Vector2::kXIdentity * (Map::kMapX - 1) * 0.5f - Vector2::kYIdentity));
	groundModelStates_.color = 0xFFFFFFFF;

}

void Ground::Update([[maybe_unused]] const float deltaTime)
{
}

void Ground::Draw(const Camera &camera) const
{
	model_->Draw(groundModelStates_.transMat, camera.GetViewOthographics(), groundModelStates_.color, BlendType::kNone);

}

void GroundEditor::Init() {
	boxMap_ = std::make_unique<MapSize>();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");

	// マップのポインタを保存
	pMap_ = GameManager::GetInstance()->GetMap();


	for (size_t i = 0; i < modelStates_.size(); i++) {
		modelStates_[i] = std::make_unique<ModelState>();
		modelStates_[i]->transform.scale = { vBlockScale->x * 0.5f,vBlockScale->x * 0.5f ,vBlockScale->y * 0.5f };
	}

}

void GroundEditor::Update([[maybe_unused]] const float deltaTime) {

}

void GroundEditor::Draw([[maybe_unused]] const Camera &camera) const {

#ifdef _DEBUG

	for (size_t i = 0; i < modelStates_.size(); i++) {
		if ((modelStates_[i]->color & 0xFF) != 0x00) {	// 色のアルファ色が無色でない場合は描画
			model_->Draw(modelStates_[i]->transform.matWorld_, camera.GetViewOthographics(), modelStates_[i]->color, BlendType::kNormal);
		}
	}

#endif // _DEBUG

}

//bool Ground::Debug(const char* const str){
//
//	return false;
//}

void GroundEditor::TransferBoxData() {


	for (size_t x = 0; x < kMapX; x++) {
		// 箱の状態
		GroundInfo boxInfo = static_cast<GroundInfo>((*boxMap_)[x]);

		// 現在のモデル
		modelStates_[x]->transform.translate = GetGrobalPos((int32_t)(x), -1, 0);
		modelStates_[x]->transform.CalcMatrix();

		if (boxInfo.isConstruction) {
			if (boxInfo.isMultiSelect_) {
				modelStates_[x]->color = 0xffff0055;
			}
			else {
				modelStates_[x]->color = 0x00ff0055;
			}
		}
		else {
			if (boxInfo.isMultiSelect_) {
				modelStates_[x]->color = 0xff000055;
			}
			else {
				modelStates_[x]->color = 0xffffff00;
			}
		}

	}


}

void GroundEditor::MultiReset() {
	for (size_t x = 0; x < kMapX; x++) {
		// 箱の状態
		GroundInfo &boxInfo = ((*boxMap_)[x]);
		boxInfo.isMultiSelect_ = false;
	}
}

const GroundEditor::GroundInfo GroundEditor::GetGroundInfo(const Vector3 &localPos) const {

	// もしマップ外に行っていた場合虚無
	if (localPos.x < 0.f or localPos.y < 0.f or localPos.z < 0.f or localPos.x >= Map::kMapX or localPos.y >= 1 or localPos.z >= 1) {
		return GroundInfo();
	}

	return GroundInfo();
}