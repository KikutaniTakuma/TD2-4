#include "BlockMap.h"

#include "../SoLib/SoLib/SoLib_Traits.h"
#include "Drawers/DrawerManager.h"
#include "SoLib/Math/Math.hpp"
#include <imgui.h>
#include "Game/GameManager/GameManager.h"

void BlockMap::Init()
{

	pTexture2d_ = DrawerManager::GetInstance()->GetTexture2D();
	blockMap_ = std::make_unique<Block2dMap>();
	blockStatesMap_ = std::make_unique<Map2dMap<std::unique_ptr<BlockStatus>>>();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");
	model_ = drawerManager->GetModel("Resources/Cube.obj");

	//ground_ = std::make_unique<Ground>();
	//ground_->Init();

}

void BlockMap::Update([[maybe_unused]] const float deltaTime)
{

	for (auto &blockStatusLine : *blockStatesMap_) {
		for (auto &blockStatus : blockStatusLine) {
			if (blockStatus) {
				blockStatus->Update(deltaTime);
			}
		}
	}
}

void BlockMap::Draw([[maybe_unused]] const Camera &camera) const
{
	Lamb::SafePtr texManager = TextureManager::GetInstance();
	const uint32_t whiteTex = texManager->GetWhiteTex();
	uint32_t blockTex = 0;

	const auto &breakTimer = GameManager::GetInstance()->GetBreakTimer();
	bool isDraw = std::fmodf(breakTimer.GetProgress(), 0.2f) > 0.1f;

	int32_t yi = 0;
	for (const auto &modelStateArr : modelStateMap_) {
		int32_t xi = 0;
		for (const auto &modelState : modelStateArr) {
			if (modelState) {
				for (uint32_t typeIndex = 1; typeIndex < static_cast<uint32_t>(Block::BlockType::kMax); typeIndex++) {
					if (modelState->color == Block::kBlockColor_[typeIndex]) {
						if (modelState->color == damageColor_ and hitMap_[yi][xi]) {

							blockTex = whiteTex;
						}
						else {
							blockTex = Block::kTextures_[typeIndex - 1];
						}
					}
				}
				pTexture2d_->Draw(modelState->transMat, Mat4x4::kIdentity, camera.GetViewOthographics(), blockTex, 0xFFFFFFFF, BlendType::kNone);
			}
			// 破壊フラグが立っていたら
			if (isDraw and breakBlockMap_[yi][xi]) {
				Mat4x4 affine = SoLib::Math::Affine(Vector3::kIdentity, Vector3::kZero, Vector3{ GetGlobalPos(Vector2{static_cast<float>(xi), static_cast<float>(yi)}), -6.f });

				pTexture2d_->Draw(affine, Mat4x4::kIdentity, camera.GetViewOthographics(), whiteTex, 0xFFFFFFFF, BlendType::kNone);

			}


			xi++;
		}
		yi++;
	}
	//ground_->Draw(camera);
}

bool BlockMap::Debug(const char *const str)
{
	bool isChange = false;
	str;
#ifdef _DEBUG

	if (ImGui::TreeNode(str)) {

		//for (size_t y = 0; y < kMapY; y++) {
		//	//
		//	// if (ImGui::TreeNode(("階層" + SoLib::to_string(y)).c_str())) {
		//	// for (size_t z = 0; z < kMapZ; z++) {
		//	for (size_t x = 0; x < kMapX; x++) {
		//		isChange |= ImGui::Checkbox(("##Checkbox" + std::to_string(y) + ' ' + std::to_string(x)).c_str(), &reinterpret_cast<bool &>((*blockMap_)[y][x]));
		//		if (x != 9) {
		//			ImGui::SameLine();
		//		}
		//	}
		//	//}
		//	// ImGui::TreePop();
		//	//}
		//}

		isChange |= SoLib::ImGuiWidget(&vCenterDiff_);

		ImGui::TreePop();
	}

#endif // _DEBUG

	return isChange;
}

void BlockMap::TransferBoxData()
{
	//const auto &hitMap = GameManager::GetInstance()->GetHitMap();

	for (int32_t yi = 0; yi < kMapY; yi++) {
		for (int32_t xi = 0; xi < kMapX; xi++) {
			// ボックスの状態
			const auto &box = (*blockMap_)[yi][xi];

			// モデルのデータ
			auto &modelState = modelStateMap_[yi][xi];

			// ボックスが存在する場合は実体を作成
			if (box) {
				// もしすでに実在したら生成しない
				if (not modelState) {
					modelState = std::make_unique<MatrixModelState>();
				}
				// 描画先の座標
				const Vector2 drawPos = GetGlobalPos(Vector2{ static_cast<float>(xi), static_cast<float>(yi) } /*+ (*blockStatesMap_)[yi][xi]->drawOffset_*/);
				modelState->transMat = SoLib::Math::Affine(Vector3::kIdentity, Vector3::kZero, { drawPos, -1.f });
				// 色を指定する
				modelState->color = box.GetColor();

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
	// for (auto &house : houseList_) {

	//	// 現在のモデル
	//	house.houseModelState_.transMat = SoLib::Math::Affine(Vector3{ static_cast<float>(*vEnemyHouseWidth_),1,1 } + Vector3::kIdentity * 0.1f, Vector3::kZero, GetGrobalPos(Vector2{ static_cast<float>(house.xPos_), -1.f }));

	//}
}

void BlockMap::SetDamageColor(uint32_t color)
{
	damageColor_ = color;
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
			auto &box = (*blockMap_)[yPos][xPos];
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

const Block::BlockType BlockMap::GetBlockType(const Vector2 localPos) const
{
	// もしマップ外に行っていた場合虚無
	if (IsOutSide(localPos)) {
		return Block::BlockType::kNone;
	}

	// ブロックのデータを返す
	return (*blockMap_)[int(localPos.y)][int(localPos.x)].GetBlockType();
}

const Block::BlockType BlockMap::GetBlockType(const POINTS localPos) const
{
	// もしマップ外に行っていた場合虚無
	if (IsOutSide(localPos)) {
		return Block::BlockType::kNone;
	}

	// ブロックのデータを返す
	return (*blockMap_)[int(localPos.y)][int(localPos.x)].GetBlockType();
}

bool BlockMap::IsOutSide(const Vector2 localPos)
{
	return localPos.x < 0.f or localPos.y < 0.f or localPos.x >= BlockMap::kMapX or localPos.y >= BlockMap::kMapY;
}

bool BlockMap::IsOutSide(const POINTS localPos)
{
	return localPos.x < 0 or localPos.y < 0 or localPos.x >= BlockMap::kMapX or localPos.y >= BlockMap::kMapY;
}


void BlockMap::BreakBlock(POINTS localPos)
{
	if (IsOutSide(localPos)) {
		return;
	}
	auto &targetBlock = blockMap_->at(localPos.y).at(localPos.x);
	// ブロックがあるなら破壊
	if (targetBlock) {
		targetBlock.SetIsDestroy(false);
		targetBlock.SetDamage(0);
		targetBlock.SetBlockType(Block::BlockType::kNone);
		damageColor_ = 0;
		blockStatesMap_->at(localPos.y).at(localPos.x).reset();
	}
}

void BlockMap::BreakUpdate([[maybe_unused]] const float deltaTime)
{

}

BlockMap::BlockBitMap &&BlockMap::FindChainBlocks(POINTS localPos, const Block::BlockType originType, const std::unordered_set<POINTS> &set, BlockBitMap &&result) const
{
	static constexpr std::array<POINTS, 4u> kMoveDir{
		{{-1, 0},
		 {1, 0},
		 {0, -1},
		 {0, 1}} };

	// 今のブロックのタイプ
	const Block::BlockType localType = GetBlockType(localPos);
	// もし虚空なら終わる
	if (localType == Block::BlockType::kNone and not set.contains(localPos)) {
		return std::move(result);
	}
	result[localPos.y][localPos.x] = true;

	// 上下左右に移動してみる
	for (const POINTS moveDir : kMoveDir) {
		// 移動先の座標
		POINTS targetPos;
		targetPos.x = localPos.x + moveDir.x;
		targetPos.y = localPos.y + moveDir.y;

		// 移動先の情報
		const Block::BlockType targetType = GetBlockType(targetPos);
		// ステージ外か
		bool isOutMap = IsOutSide(targetPos);
		// もしステージ外なら飛ばす
		if (isOutMap) { continue; }

		// 移動先のブロックが現在と一致してたら
		if ((set.contains(targetPos) or targetType == originType) and result[targetPos.y][targetPos.x] == false) {
			result = FindChainBlocks(targetPos, originType, set, std::move(result));
		}
	}

	return std::move(result);
}
