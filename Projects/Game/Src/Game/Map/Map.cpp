#include "BlockMap.h"

#include "../SoLib/SoLib/SoLib_Traits.h"
#include <imgui.h>
#include"Drawers/DrawerManager.h"
#include "SoLib/Math/Math.hpp"

void BlockMap::Init()
{
	boxMap_ = std::make_unique<Block2dMap>();
	blockStatesMap_ = std::make_unique<Map2dMap<std::unique_ptr<BlockStatus>>>();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");

	ground_ = std::make_unique<Ground>();
	ground_->Init();

}

void BlockMap::Update([[maybe_unused]] const float deltaTime) {

	for (auto &blockStatusLine : *blockStatesMap_) {
		for (auto &blockStatus : blockStatusLine) {
			if (blockStatus) {
				blockStatus->Update(deltaTime);
			}
		}
	}

}

void BlockMap::Draw([[maybe_unused]] const Camera &camera) const {
	for (const auto &modelStateArr : modelStateMap_) {
		for (const auto &modelState : modelStateArr) {
			if (modelState) {
				model_->Draw(modelState->transMat, camera.GetViewOthographics(), modelState->color, BlendType::kNone);
			}
		}
	}
	ground_->Draw(camera);
}

bool BlockMap::Debug(const char *const str)
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

void BlockMap::TransferBoxData()
{

	for (int32_t yi = 0; yi < kMapY; yi++) {
		for (int32_t xi = 0; xi < kMapX; xi++) {
			// ボックスの状態
			const auto &box = (*boxMap_)[yi][xi];

			// モデルのデータ
			auto &modelState = modelStateMap_[yi][xi];

			// ボックスが存在する場合は実体を作成
			if (box) {
				// 描画先の座標
				const Vector2 drawPos = GetGlobalPos(Vector2{ static_cast<float>(xi), static_cast<float>(yi) } + (*blockStatesMap_)[yi][xi]->drawOffset_);
				// もしすでに実在したら生成しない
				if (not modelState) {
					modelState = std::make_unique<MatrixModelState>();

				}
				modelState->transMat = SoLib::Math::Affine(Vector3{ vBlockScale_->x, vBlockScale_->y, vBlockScale_->y } *0.5f, Vector3::kZero, { drawPos, -1.f });
				// 色を指定する
				modelState->color = Block::kBlockColor_[static_cast<uint32_t>(box)];
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

	//// 拠点のデータの転送
	//for (auto &house : houseList_) {

	//	// 現在のモデル
	//	house.houseModelState_.transMat = SoLib::Math::Affine(Vector3{ static_cast<float>(*vEnemyHouseWidth_),1,1 } + Vector3::kIdentity * 0.1f, Vector3::kZero, GetGrobalPos(Vector2{ static_cast<float>(house.xPos_), -1.f }));

	//}

}

void BlockMap::SetBlocks(Vector2 centerPos, Vector2 size, Block::BlockType boxType)
{
	// 半径
	Vector2 halfSize = Vector2{ std::floor(size.x * 0.5f), std::floor(size.y * 0.5f) };

	// 中心座標をより正しい形にする
	Vector2 calcCenterPos;

	for (int32_t i = 0; i < 2; i++) {

		SoLib::begin(calcCenterPos)[i] = std::floor(SoLib::begin(centerPos)[i]) + SoLib::begin(halfSize)[i] - ((SoLib::begin(size)[i] - 1.f) * 0.5f);

	}



	for (int32_t yi = 0; yi < static_cast<int32_t>(size.y); yi++) {
		for (int32_t xi = 0; xi < static_cast<int32_t>(size.x); xi++) {

			int32_t xPos = static_cast<int32_t>(centerPos.x) - xi + static_cast<int32_t>(halfSize.x);
			int32_t yPos = static_cast<int32_t>(centerPos.y) - yi + static_cast<int32_t>(halfSize.y);

			// 範囲外なら飛ばす
			if (xPos < 0 or yPos < 0 or xPos >= BlockMap::kMapX or yPos >= BlockMap::kMapY) {
				continue;
			}
			// 参照を取得する
			auto &box = (*boxMap_)[yPos][xPos];
			// データを代入する
			box.SetBlockType(boxType);

			// ブロックのステータスの参照
			auto &blockState = (*blockStatesMap_)[yPos][xPos];
			// 追加するブロックのデータ
			auto setBlockState = std::make_unique<BlockStatus>();

			// データのコピー
			{
				/*setBlockState->centerPos_ = calcCenterPos;
				setBlockState->blockSize_ = size;*/
				setBlockState->localPos_ = Vector2{ static_cast<float>(xPos), static_cast<float>(yPos) };
			}

			// データの転送
			blockState = std::move(setBlockState);

		}
	}


}

const Block::BlockType BlockMap::GetBoxType(const Vector2 localPos) const {
	// もしマップ外に行っていた場合虚無
	if (IsOutSide(localPos)) {
		return Block::BlockType::kNone;
	}

	// ブロックのデータを返す
	return (*boxMap_)[int(localPos.y)][int(localPos.x)].GetBlockType();
}

bool BlockMap::IsOutSide(const Vector2 &localPos)
{
	return localPos.x < 0.f or localPos.y < 0.f or localPos.x >= BlockMap::kMapX or localPos.y >= BlockMap::kMapY;
}
