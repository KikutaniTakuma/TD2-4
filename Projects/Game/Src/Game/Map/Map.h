#pragma once
#include <array>
#include <cstdint>
#include "../SoLib/Containers/VItem.h"
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include <list>
#include <Drawers/Model/Model.h>
#include <Camera/Camera.h>

class Map
{
public:

	enum class BoxType : uint32_t {
		kNone,	// 虚空
		kBox,	// 箱
	};
	inline static constexpr int32_t kMapX = 10u, kMapY = 5u, kMapZ = 10u;

	// 箱の配列 [y][z][x]
	using MapSize = std::array<std::array<std::array<BoxType, kMapX>, kMapZ>, kMapY>;

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

public:
	/// @brief 3次元配列の取得
	/// @return 三次元配列
	MapSize *GetBlockMap() { return boxMap_.get(); }

	Vector3 GetGrobalPos(size_t x, size_t y, size_t z) const noexcept
	{
		return Vector3{ x * vBoxDistance_->x, y * vBoxDistance_->y, -(z * vBoxDistance_->x) } - Vector3{ vBoxDistance_->x * ((kMapX - 1) / 2.f), 0, -(vBoxDistance_->x * ((kMapZ - 1) / 2.f)) };
	}

	Vector3 LocalPos(const Vector3 &gPos) const noexcept
	{
		return Vector3{ gPos.x / vBoxDistance_->x, gPos.y / vBoxDistance_->y, -(gPos.z / vBoxDistance_->x) } + Vector3{ vBoxDistance_->x / ((kMapX - 1) / 2.f), 0, -(vBoxDistance_->x / ((kMapZ - 1) / 2.f)) };
	}

private:

	// 箱の配列 [y][z][x]
	std::unique_ptr<MapSize> boxMap_;
	// 箱の数
	size_t boxCount_;

	inline static SoLib::VItem<"ブロックの間隔", Vector2> vBoxDistance_{ {1, 3} };
	inline static SoLib::VItem<"ブロックのサイズ", float> vBlockScale{ 1.f };

	std::list<std::unique_ptr<Model>> models_;

};