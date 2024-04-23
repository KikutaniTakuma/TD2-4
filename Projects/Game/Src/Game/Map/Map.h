#pragma once
#include <array>
#include <cstdint>
#include "../SoLib/Containers/VItem.h"
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include <list>
#include <Drawers/Model/Model.h>
#include <Camera/Camera.h>
#include <bitset>

class Map
{
public:

	// 拠点1つごとの情報
	struct HouseInfo {

		// x座標
		int32_t xPos_{};

		// モデルの表示情報
		ModelState houseModelState_;

		//複数選択の対象かどうか
		bool isMultiSelect_;	// エディター側が持ってたほうが個人的にありがたい｡
	};

	// ボックスに紐づいている情報
	enum class BoxType : uint32_t {
		kNone,	// 虚空
		kBox,	// 箱
	};

	inline static constexpr int32_t kMapX = 30u, kMapY = 20u;

	// 拠点のリスト
	using HouseList = std::list<HouseInfo>;

	// 箱の配列 [y][x]
	using MapSize = std::array<std::array<BoxType, kMapX>, kMapY>;

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

	/// <summary>
	/// マルチ設定のリセット
	/// </summary>
	void MultiReset();

	//const HouseInfo& GetHouseInfo(const int localPosX) const;

	const BoxType GetBoxType(const Vector2 &localPos) const;

	bool IsOutSide(const Vector2 &localPos) const;

	inline void SetDraingFlag(const std::bitset<kMapY> &flag) noexcept { isFloorDrawing_ = flag; }
	inline const std::bitset<kMapY> &GetDraingFlag() const noexcept { return isFloorDrawing_; }

public:
	/// @brief 2次元配列の取得
	/// @return 二次元配列
	MapSize *GetBlockMap() { return boxMap_.get(); }

	/// @brief 拠点のリストを返す
	/// @return 拠点のリスト
	HouseList *GetHouseList() { return &houseList_; }

	static Vector3 GetGrobalPos(int32_t x, int32_t y) noexcept
	{
		return Vector3{ x * vBoxDistance_->x, y * vBoxDistance_->y,0 } - Vector3{ vBoxDistance_->x * ((kMapX - 1) / 2.f), 0, 0 };
	}

	static Vector3 LocalPos(const Vector3 &gPos) noexcept
	{
		return Vector3{ gPos.x / vBoxDistance_->x, gPos.y / vBoxDistance_->y,0 } + Vector3{ vBoxDistance_->x / ((kMapX - 1) / 2.f), 0, 0 };
	}

	static float GetMapDistance() {
		return vBoxDistance_->y;
	}

private:

	// 箱の配列 [y][x]
	std::unique_ptr<MapSize> boxMap_;
	// 拠点のリスト
	HouseList houseList_;

	// 箱の数
	size_t boxCount_{};

	std::bitset<kMapY> isFloorDrawing_{ 0b1 };

	inline static SoLib::VItem<"ブロックの間隔", Vector2> vBoxDistance_{ {1, 1} };
	inline static SoLib::VItem<"ブロックのサイズ", Vector2> vBlockScale{ {1.f,10.0f} };

	Model *model_;
	/// <summary>
	/// モデルの情報
	/// </summary>
	std::array<std::unique_ptr<ModelState>, kMapX> modelStates_;

};