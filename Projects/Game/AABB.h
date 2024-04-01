#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include <vector>
#include <array>

struct AABB {
	Vector3 min;
	Vector3 max;

	AABB AddPos(const Vector3 &vec) const;
	AABB Extend(const Vector3 &vec) const;

	Vector3 GetCentor() const;
	Vector3 GetRadius() const;

	/// @brief 法線を取得する
	/// @param surfacePoint 接触している面の座標
	/// @return 法線情報
	const Vector3& GetNormal(const Vector3 &surfacePoint) const;

	/// @brief Transform行列の生成
	/// @return transform行列
	Mat4x4 TransMat() const;

	/// @brief 新規生成
	/// @param origin 原点
	/// @param radius 半径
	/// @return AABB構造体
	static AABB Create(const Vector3 &origin, const Vector3 &radius);

	bool ImGuiDebug(const char *const group);
	const AABB &SwapSelf();

	/// @brief 全ての頂点情報を取得する
	/// @return 各種頂点 [ 下面4つ , 上面4つ]
	std::array<Vector3, 8u> GetVertex() const;
};