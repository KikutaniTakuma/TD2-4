#include "Map.h"

#include "../SoLib/SoLib/SoLib_Traits.h"
#include <imgui.h>
#include"Drawers/DrawerManager.h"
#include "SoLib/Math/Math.hpp"

void Map::Init()
{
	boxMap_ = std::make_unique<Block2dMap>();
	houseList_.clear();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");

	ground_ = std::make_unique<Ground>();
	ground_->Init();

}

void Map::Update([[maybe_unused]] const float deltaTime) {

}

void Map::Draw([[maybe_unused]] const Camera &camera) const {
	for (const auto &modelStateArr : modelStateMap_) {
		for (const auto &modelState : modelStateArr) {
			if (modelState) {
				model_->Draw(modelState->transMat, camera.GetViewProjection(), modelState->color, BlendType::kNormal);
			}
		}
	}

	ground_->Draw(camera);
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
	ground_->TransferBoxData();

	for (int32_t yi = 0; yi < kMapY; yi++) {
		for (int32_t xi = 0; xi < kMapX; xi++) {
			// ボックスの状態
			const BoxType box = (*boxMap_)[yi][xi];

			// モデルのデータ
			auto &modelState = modelStateMap_[yi][xi];

			// ボックスが存在する場合は実体を作成
			if (box != BoxType::kNone and box != BoxType::kMax) {
				// もしすでに実在したら生成しない
				if (not modelState) {
					modelState = std::make_unique<MatrixModelState>();
					modelState->transMat = SoLib::Math::Affine(Vector3{ vBlockScale_->x, vBlockScale_->y, vBlockScale_->y } *0.5f, Vector3::kZero, GetGrobalPos(Vector2{ static_cast<float>(xi), static_cast<float>(yi) }));
				}
				// 色を指定する
				modelState->color = kBoxColors_[static_cast<uint32_t>(box)];
			}
			// もしボックスが存在しない場合は
			else {
				// モデルデータを破棄する
				if (modelState) {
					modelState.reset();
				}
			}
		}
	}

	// 拠点のデータの転送
	for (auto &house : houseList_) {

		// 現在のモデル
		house.houseModelState_.transMat = SoLib::Math::Affine(Vector3{ static_cast<float>(*vEnemyHouseWidth_),1,1 } + Vector3::kIdentity * 0.1f, Vector3::kZero, GetGrobalPos(Vector2{ static_cast<float>(house.xPos_), -1.f }));

	}

}

void Map::SetBlocks(Vector2 centerPos, Vector2 size, BoxType boxType)
{

	for (int32_t yi = 0; yi < static_cast<int32_t>(size.y); yi++) {
		for (int32_t xi = 0; xi < static_cast<int32_t>(size.x); xi++) {

			int32_t xPos = static_cast<int32_t>(centerPos.x) - xi + static_cast<int32_t>(size.x) / 2;
			int32_t yPos = static_cast<int32_t>(centerPos.y) - yi + static_cast<int32_t>(size.y) / 2;

			// 範囲外なら飛ばす
			if (xPos < 0 or yPos < 0 or xPos >= Map::kMapX or yPos >= Map::kMapY) {
				continue;
			}
			// 参照を取得する
			auto &box = (*boxMap_)[yPos][xPos];
			// データを代入する
			box = boxType;


		}
	}


}

const Map::BoxType Map::GetBoxType(const Vector2 localPos) const {
	// もしマップ外に行っていた場合虚無
	if (IsOutSide(localPos)) {
		return BoxType::kNone;
	}

	// ブロックのデータを返す
	return (*boxMap_)[int(localPos.y)][int(localPos.x)];
}

bool Map::IsOutSide(const Vector2 &localPos)
{
	return localPos.x < 0.f or localPos.y < 0.f or localPos.x >= Map::kMapX or localPos.y >= Map::kMapY;
}