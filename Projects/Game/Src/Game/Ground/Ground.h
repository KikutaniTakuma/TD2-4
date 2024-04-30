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

class Map;


// 地形の描画を行うクラス。
class Ground {
public:

private:

	// 描画時のモデル
	Model *model_;

	// 床のデータ
	MatrixModelState groundModelStates_;

public:

	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;
};

class GroundEditor {
public:

	struct GroundInfo {
		//建設予定になって入るかどうか
		bool isConstruction;
		//小人は何人設置されているかどうか
		int32_t dwarfNum;
		//複数選択の大将かどうか
		bool isMultiSelect_;
	};

	inline static constexpr int32_t kMapX = 30u;

	// 床の描画の配列 [x]
	using MapSize = std::array<GroundInfo, kMapX>;

public:
	GroundEditor() = default;
	~GroundEditor() = default;


	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

public:
	/// @brief デバッグ
	/// @param str Tree名
	/// @return 変更したか
	//bool Debug(const char* const str);

	/// @brief 箱のデータをモデルに転送
	void TransferBoxData();

	/// <summary>
	/// マルチ設定のリセット
	/// </summary>
	void MultiReset();

	const GroundInfo GetGroundInfo(const Vector3 &localPos) const;

	void SetGround(Ground *ground) { pGround_ = ground; }

public:
	/// @brief 3次元配列の取得
	/// @return 三次元配列
	MapSize *GetGroundData() { return boxMap_.get(); }

	static Vector3 GetGrobalPos(size_t x, size_t y, size_t z) noexcept
	{
		return Vector3{ x * vBoxDistance_->x, y * vBoxDistance_->y, -(z * vBoxDistance_->x) } - Vector3{ vBoxDistance_->x * ((kMapX - 1) / 2.f), 0, -(vBoxDistance_->x * ((1 - 1) / 2.f)) };
	}

	static Vector3 GetGrobalPos(int32_t x, int32_t y, int32_t z) noexcept
	{
		return Vector3{ x * vBoxDistance_->x, y * vBoxDistance_->y, -(z * vBoxDistance_->x) } - Vector3{ vBoxDistance_->x * ((kMapX - 1) / 2.f), 0, -(vBoxDistance_->x * ((1 - 1) / 2.f)) };
	}

	static Vector3 LocalPos(const Vector3 &gPos) noexcept
	{
		return Vector3{ gPos.x / vBoxDistance_->x, gPos.y / vBoxDistance_->y, -(gPos.z / vBoxDistance_->x) } + Vector3{ vBoxDistance_->x / ((kMapX - 1) / 2.f), 0, -(vBoxDistance_->x / ((1 - 1) / 2.f)) };
	}

	static float GetMapDistance() {
		return vBoxDistance_->y;
	}

private:

	Map *pMap_ = nullptr;

	Lamb::SafePtr<Ground> pGround_ = nullptr;

	// 箱の配列 [x]
	std::unique_ptr<MapSize> boxMap_;
	// 箱の数
	size_t boxCount_{};

	inline static SoLib::VItem<"ブロックの間隔", Vector2> vBoxDistance_{ {1, 1} };
	inline static SoLib::VItem<"ブロックのサイズ", Vector2> vBlockScale{ {1.f,10.0f} };

	Model *model_;


	/// <summary>
	/// モデルの情報
	/// </summary>
	std::array<std::unique_ptr<ModelState>, kMapX> modelStates_;

};

