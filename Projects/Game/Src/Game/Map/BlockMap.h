#pragma once
#include "../SoLib/Containers/VItem.h"
#include "../SoLib/SoLib_Traits.h"
#include "Drawers/DrawerManager.h"
#include "Game/Ground/Ground.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "SoLib/Math/Angle.h"
#include <Camera/Camera.h>
#include <Drawers/Model/Model.h>
#include <Game/Block/Block.h>
#include <array>
#include <bitset>
#include <cstdint>
#include <functional>
#include <list>
#include <unordered_set>

inline bool operator==(const POINTS l, const POINTS r) {
	return l.x == r.x and l.y == r.y;
}

template<>
struct std::hash<POINTS> {
	size_t operator()(const POINTS &data) const {
		int32_t tmp;
		tmp = data.y;
		tmp <<= 16;
		tmp = data.x;
		return std::hash<int32_t>{}(tmp);
	}
};

class BlockMap
{
public:
	// ブロックの情報
	struct BlockStatus
	{
		// ローカル座標
		Vector2 localPos_;

		// 描画時の差分
		Vector2 drawOffset_{};
		Vector2 shakeVec_;

		// 振動の時間
		float shakeTime_ = 0.f;
		float startTime_ = 0;

		// int32_t GetWeight() const { return static_cast<int32_t>(blockSize_.x * blockSize_.y); }

		/// @brief 振動を開始する
		/// @param time 振動する時間
		/// @param power 振動の方向
		void StartShake(float time, Vector2 power)
		{
			startTime_ = time;
			shakeTime_ = time;

			shakeVec_ = power;
		}

		void Update(float deltaTime)
		{

			shakeTime_ -= deltaTime;
			if (shakeTime_ <= 0) {
				shakeTime_ = 0;
				startTime_ = 0;
			}

			Vector2 diff = {};

			if (startTime_ != 0) {
				float x = shakeTime_ / startTime_;

				diff += shakeVec_ * x * std::sin(4 * x * Angle::PI) / 8 * x;
			}
			drawOffset_ = diff;
		}
	};

	inline static constexpr int32_t kMapX = 15u, kMapY = 10u;

	// マップの配列 [y][x]
	template <SoLib::IsRealType T>
	using Map2dMap = std::array<std::array<T, kMapX>, kMapY>;

	// 箱の配列 [y][x]
	using Block2dMap = Map2dMap<Block>;

	using BlockStatusMap = Map2dMap<std::unique_ptr<BlockStatus>>;

	using BlockBitMap = std::array<std::bitset<kMapX>, kMapY>;

public:
	BlockMap() = default;
	~BlockMap() = default;

	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

public:
	/// @brief デバッグ
	/// @param str Tree名
	/// @return 変更したか
	bool Debug(const char *const str);

	/// @brief 箱のデータをモデルに転送
	void TransferBoxData();

	void SetDamageColor(uint32_t color);

	/// @brief ブロックを設置する
	/// @param centerPos 中心座標
	/// @param size 直径
	/// @param boxType ブロックのタイプ
	void SetBlocks(Vector2 centerPos, Vector2 size, Block::BlockType boxType);

	const Block::BlockType GetBlockType(const Vector2 localPos) const;
	const Block::BlockType GetBlockType(const POINTS localPos) const;

	static bool IsOutSide(const Vector2 localPos);
	static bool IsOutSide(const POINTS localPos);


	void BreakBlock(POINTS localPos);

	void BreakUpdate(const float deltaTime);

	void ProcessEnemyHouseBlocks(std::function<void(int32_t y, int32_t x)> processBlock)
	{
		for (int yi = 0; yi < BlockMap::kMapY; yi++) {
			for (int xi = -1; xi < 2; xi++) {
				processBlock(yi, xi);
			}
		}
	}

public:
	/// @brief 2次元配列の取得
	/// @return 二次元配列
	Block2dMap *GetBlockMap() { return blockMap_.get(); }

	/// @brief ブロックのステータスの二次元配列の取得
	/// @return 二次元配列
	BlockStatusMap *GetBlockStatusMap() { return blockStatesMap_.get(); }

	static Vector2 GetGlobalPos(Vector2 localPos) noexcept
	{
		return localPos - Vector2::kXIdentity * vBlockScale_->x * ((kMapX - 1) / 2.f);
	}

	static Vector2 LocalPos(const Vector2 gPos) noexcept
	{
		return Vector2{ gPos.x / vBlockScale_->x, gPos.y / vBlockScale_->y } + Vector2::kXIdentity * vBlockScale_->x / ((kMapX - 1) / 2.f);
	}

	static float GetMapDistance()
	{
		return vBlockScale_->y;
	}

	static Vector2 GetMapSize() noexcept
	{
		return Vector2{ kMapX, kMapY };
	}

	BlockBitMap &&FindChainBlocks(POINTS localPos, const std::unordered_set<POINTS> &set, BlockBitMap &&result = {}) const;

	void SetBreakMap(const BlockBitMap &map) { breakMap_ = map; }
	const BlockBitMap &GetBreakMap() const { return breakMap_; }

	void SetBreakBlockMap(const BlockBitMap &map) { breakBlockMap_ = map; }
	const BlockBitMap &GetBreakBlockMap() const { return breakBlockMap_; }

	void SetBreakBlockType(const Block::BlockType type) { breakBlockType_ = type; }
	Block::BlockType GetBreakBlockType() const { return breakBlockType_; }

	void SetHitMap(const BlockBitMap &map) { hitMap_ = map; }
	const BlockBitMap &GetHitMap() const { return hitMap_; }

	void ClearHitMap()
	{
		SetDamageColor(0);
		for (auto &line : hitMap_) {
			line.reset();
		}
	}


private:

	BlockBitMap hitMap_;

	BlockBitMap breakMap_;

	Block::BlockType breakBlockType_ = Block::BlockType::kNone;
	BlockBitMap breakBlockMap_;

private:
	// 箱の配列 [y][x]
	std::unique_ptr<Block2dMap> blockMap_;
	// ブロックの情報
	std::unique_ptr<BlockStatusMap> blockStatesMap_;

	// 拠点のリスト
	// HouseList houseList_;

	inline static SoLib::VItem<"ブロックのサイズ", Vector2> vBlockScale_{ {1.f, 1.f} };
	inline static SoLib::VItem<"敵拠点の横幅", int32_t> vEnemyHouseWidth_{ 3 };

	Model *model_;

	/// <summary>
	/// モデルの情報
	/// </summary>
	Map2dMap<std::unique_ptr<MatrixModelState>> modelStateMap_;

	// 床のクラス
	std::unique_ptr<Ground> ground_;

	Texture2D *pTexture2d_ = nullptr;



	//変数
//攻撃を受けたかどうか
	bool isDamege_ = false;

	//ダメージを受けたブロックの色
	uint32_t damageColor_;
};