#pragma once
#include <array>
#include <cstdint>
#include "../SoLib/Containers/VItem.h"
#include "../SoLib/SoLib_Traits.h"
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include <list>
#include <Drawers/Model/Model.h>
#include <Camera/Camera.h>
#include <bitset>
#include"Game/Ground/Ground.h"

class Map
{
public:

	// 拠点1つごとの情報
	struct HouseInfo {

		// x座標
		int32_t xPos_{};

		// モデルの表示情報
		MatrixModelState houseModelState_;

	};
	// ブロックの情報
	struct BlockStatus {
		// 中心の座標
		Vector2 centerPos_;
		// ブロックの大きさ
		Vector2 blockSize_;
		// ローカル座標
		Vector2 localPos_;

		int32_t GetWeight() const { return static_cast<int32_t>(blockSize_.x * blockSize_.y); }

	};


	// ボックスに紐づいている情報
	enum class BoxType : uint32_t {
		kNone,			// 虚空
		kGroundBlock,	// プレイヤ側のブロック
		kEnemyBlock,    // 敵側のブロック

		kMax,           // 最大値
	};

	inline static const std::array<uint32_t, static_cast<uint32_t>(BoxType::kMax)> kBoxColors_{ 0x00000000, 0xFFFFFFFF, 0xFF0000FF };

	inline static constexpr int32_t kMapX = 30u, kMapY = 20u;

	// 拠点のリスト
	using HouseList = std::list<HouseInfo>;

	// マップの配列 [y][x]
	template<SoLib::IsRealType T>
	using Map2dMap = std::array<std::array<T, kMapX>, kMapY>;

	// 箱の配列 [y][x]
	using Block2dMap = Map2dMap<BoxType>;

	using BlockStatusMap = Map2dMap<std::unique_ptr<BlockStatus>>;

public:
	Map() = default;
	~Map() = default;


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

	/// @brief ブロックを設置する
	/// @param centerPos 中心座標
	/// @param size 直径
	/// @param boxType ブロックのタイプ
	void SetBlocks(Vector2 centerPos, Vector2 size, BoxType boxType);

	const BoxType GetBoxType(const Vector2 localPos) const;

	static bool IsOutSide(const Vector2 &localPos);

	inline void SetDraingFlag(const std::bitset<kMapY> &flag) noexcept { isFloorDrawing_ = flag; }
	inline const std::bitset<kMapY> &GetDraingFlag() const noexcept { return isFloorDrawing_; }


	void AddHouse(int32_t xCenter);


	Map::HouseInfo GetNearestHouse(int32_t x) const;
	
public:
	/// @brief 2次元配列の取得
	/// @return 二次元配列
	Block2dMap *GetBlockMap() { return boxMap_.get(); }

	/// @brief ブロックのステータスの二次元配列の取得
	/// @return 二次元配列
	BlockStatusMap *GetBlockStatusMap() { return blockStatesMap_.get(); }


	/// @brief 地面のエディターを取得する
	/// @return エディタを返す
	GroundEditor *GetGroundEditor() { return groundEditor_.get(); }

	/// @brief 拠点のリストを返す
	/// @return 拠点のリスト
	HouseList *GetHouseList() { return &houseList_; }

	static Vector2 GetGrobalPos(Vector2 localPos) noexcept
	{
		return Vector2{ localPos.x * vBlockScale_->x, localPos.y * vBlockScale_->y } - Vector2::kXIdentity * vBlockScale_->x * ((kMapX - 1) / 2.f);
	}

	static Vector2 LocalPos(const Vector2 gPos) noexcept
	{
		return Vector2{ gPos.x / vBlockScale_->x, gPos.y / vBlockScale_->y } + Vector2::kXIdentity * vBlockScale_->x / ((kMapX - 1) / 2.f);
	}

	static float GetMapDistance() {
		return vBlockScale_->y;
	}

	static Vector2 GetMapSize() noexcept {
		return Vector2{ kMapX, kMapY };
	}

private:

	void GetHouseData();

private:

	// 箱の配列 [y][x]
	std::unique_ptr<Block2dMap> boxMap_;
	// ブロックの情報
	std::unique_ptr<BlockStatusMap> blockStatesMap_;

	// 拠点のリスト
	HouseList houseList_;

	// 箱の数
	size_t boxCount_{};

	std::bitset<kMapY> isFloorDrawing_{ 0b1 };

	inline static SoLib::VItem<"ブロックのサイズ", Vector2> vBlockScale_{ {1.f,1.f} };
	inline static SoLib::VItem<"敵拠点の横幅", int32_t> vEnemyHouseWidth_{ 3 };

	Model *model_;

	/// <summary>
	/// モデルの情報
	/// </summary>
	Map2dMap<std::unique_ptr<MatrixModelState>> modelStateMap_;

	// 床のクラス
	std::unique_ptr<Ground> ground_;

	// 床のエディタークラス
	std::unique_ptr<GroundEditor> groundEditor_;
};