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


	struct BoxInfo{
		//建設予定になって入るかどうか
		bool isConstruction;
		//小人は何人設置されているかどうか
		int32_t dwarfNum;
		//複数選択の大将かどうか
		bool isMultiSelect_;
	};

	enum class BoxType : uint32_t {
		kNone,	// 虚空
		kBox,	// 箱
	};
	inline static constexpr int32_t kMapX = 20u, kMapY = 1u, kMapZ = 1u;

	// 箱の配列 [y][z][x]
	using MapSize = std::array<std::array<std::array<BoxInfo, kMapX>, kMapZ>, kMapY>;

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

	const BoxInfo GetBoxInfo(const Vector3 &localPos) const;

	const BoxType GetBoxType(const Vector3& localPos) const;

	inline void SetDraingFlag(const std::bitset<kMapY>& flag) noexcept { isFloorDrawing_ = flag; }
	inline const std::bitset<kMapY>& GetDraingFlag() const noexcept { return isFloorDrawing_; }

public:
	/// @brief 3次元配列の取得
	/// @return 三次元配列
	MapSize *GetBlockMap() { return boxMap_.get(); }

	static Vector3 GetGrobalPos(size_t x, size_t y, size_t z) noexcept
	{
		return Vector3{ x * vBoxDistance_->x, y * vBoxDistance_->y, -(z * vBoxDistance_->x) } - Vector3{ vBoxDistance_->x * ((kMapX - 1) / 2.f), 0, -(vBoxDistance_->x * ((kMapZ - 1) / 2.f)) };
	}

	static Vector3 LocalPos(const Vector3 &gPos) noexcept
	{
		return Vector3{ gPos.x / vBoxDistance_->x, gPos.y / vBoxDistance_->y, -(gPos.z / vBoxDistance_->x) } + Vector3{ vBoxDistance_->x / ((kMapX - 1) / 2.f), 0, -(vBoxDistance_->x / ((kMapZ - 1) / 2.f)) };
	}

	static float GetMapDistance() {
		return vBoxDistance_->y;
	}

private:

	// 箱の配列 [y][z][x]
	std::unique_ptr<MapSize> boxMap_;
	// 箱の数
	size_t boxCount_{};

	std::bitset<kMapY> isFloorDrawing_{ 0b1 };

	inline static SoLib::VItem<"ブロックの間隔", Vector2> vBoxDistance_{ {1, 1} };
	inline static SoLib::VItem<"ブロックのサイズ", Vector2> vBlockScale{ {1.f,10.0f} };

	Model* model_;
	/// <summary>
	/// モデルの情報
	/// </summary>
	std::array<std::unique_ptr<ModelState>,kMapX> modelStates_;

};