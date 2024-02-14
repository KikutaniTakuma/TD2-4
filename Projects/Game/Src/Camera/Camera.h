#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include <string>

/// <summary>
/// カメラ
/// </summary>
class Camera {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Camera() noexcept;
	Camera(const Camera& right) noexcept;
	Camera(Camera&& right) noexcept;
	~Camera() noexcept = default;


/// <summary>
///  代入演算子
/// </summary>
public:
	Camera& operator=(const Camera& right) noexcept = default;
	Camera& operator=(Camera&& right) noexcept = default;


/// <summary>
/// メンバ関数
/// </summary>
public:
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 行列更新
	/// </summary>
	/// <param name="gazePoint">デバッグ時の起点</param>
	void Update(const Vector3& gazePoint);
	/// <summary>
	/// 行列更新
	/// </summary>
	/// <param name="worldMat">カメラの親のワールド行列</param>
	void Update(const Mat4x4& worldMat);

	/// <summary>
	/// ビュー行列取得
	/// </summary>
	/// <returns></returns>
	const Mat4x4& GetView() const {
		return view_;
	}

	/// <summary>
	/// プロジェクション行列取得
	/// </summary>
	/// <returns></returns>
	const Mat4x4& GetProjection() const {
		return projection_;
	}

	/// <summary>
	/// 平行投影
	/// </summary>
	/// <returns></returns>
	const Mat4x4& GetOthographics() const {
		return othograohics_;
	}

	/// <summary>
	/// 透視投影行列を取得
	/// </summary>
	/// <returns>透視投影行列</returns>
	inline const Mat4x4& GetViewProjection() const noexcept {
		return viewProjecction_;
	}
	
	/// <summary>
	/// 平衡投影行列を取得
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewOthographics() const noexcept {
		return viewOthograohics_;
	}

	/// <summary>
	/// カメラのポジションを取得
	/// </summary>
	/// <returns>カメラのポジション</returns>
	const Vector3& GetPos() const {
		return worldPos_;
	}

	/// <summary>
	/// 透視投影行列にビューポートをかけたものを取得
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewProjectionVp() const noexcept {
		return viewProjecctionVp_;
	}
	/// <summary>
	/// 平衡投影行列ビューポートをかけたものを取得
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewOthographicsVp() const noexcept {
		return viewOthograohicsVp_;
	}

	void Debug(const std::string& guiName);


/// <summary>
/// メンバ変数
/// </summary>
public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

	Vector3 offset;

	float drawScale;

protected:
	Vector3 worldPos_;

protected:
	static constexpr float kNearClip_ = 0.01f;

public:
	float farClip;
	float fov;

protected:
	Mat4x4 view_;
	Mat4x4 projection_;
	Mat4x4 othograohics_;

	Mat4x4 viewProjecction_;
	Mat4x4 viewOthograohics_;

	Mat4x4 viewProjecctionVp_;
	Mat4x4 viewOthograohicsVp_;
};