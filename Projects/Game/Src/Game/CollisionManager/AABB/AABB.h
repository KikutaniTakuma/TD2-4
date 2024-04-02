#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include <vector>
#include <array>

struct AABB {
	// 最小座標
	Vector3 min;
	// 最大座標
	Vector3 max;

	/// @brief 両方の座標を移動する
	/// @param vec 移動ベクトル
	/// @return 移動したAABB
	AABB AddPos(const Vector3 &vec) const;

	/// @brief 指定した方向への拡張
	/// @param vec 拡張させるベクトル
	/// @return 拡張したAABB
	AABB Extend(const Vector3 &vec) const;

	/// @brief 中心座標の取得
	/// @return 中心座標
	Vector3 GetCentor() const;

	/// @brief 半径の取得
	/// @return 半径
	Vector3 GetRadius() const;

	/// @brief 法線を取得する
	/// @param surfacePoint 接触している面の座標
	/// @return 法線情報
	const Vector3& GetNormal(const Vector3 &surfacePoint) const;

	/// @brief Transform行列の生成
	/// @return transform行列
	Mat4x4 MakeTransMat() const;

	/// @brief 新規生成
	/// @param origin 原点
	/// @param radius 半径
	/// @return AABB構造体
	static AABB Create(const Vector3 &origin, const Vector3 &radius);

	/// @brief ImGui表示
	/// @param group グループ名
	/// @return 操作した場合 true
	bool ImGuiDebug(const char *const group);

	/// @brief 大小の修正
	/// @return 修正したAABB
	const AABB &SwapSelf();

	/// @brief 全ての頂点情報を取得する
	/// @return 各種頂点 [ 下面4つ , 上面4つ]
	std::array<Vector3, 8u> GetVertex() const;
};