#pragma once

#include <array>
#include <concepts>
#include <string>
#include <initializer_list>
#include "Vector4.h"

class Vector3;

/// <summary>
/// 4x4行列
/// </summary>
class Mat4x4 final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Mat4x4();
	Mat4x4(const Mat4x4& mat);
	Mat4x4(Mat4x4&& mat) noexcept;
	Mat4x4(const std::initializer_list<float>& num);
	Mat4x4(const std::array<Vector4, 4>& num);
	Mat4x4(const std::array<float, 16>& num);

public:
	~Mat4x4() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	Mat4x4& operator=(const Mat4x4& mat);
	Mat4x4& operator=(Mat4x4&& mat) noexcept;
	Mat4x4 operator*(const Mat4x4& mat) const;
	Mat4x4& operator*=(const Mat4x4& mat);

	Mat4x4 operator+(const Mat4x4& mat) const;
	Mat4x4& operator+=(const Mat4x4& mat);
	Mat4x4 operator-(const Mat4x4& mat) const;
	Mat4x4& operator-=(const Mat4x4& mat);

	template<std::integral T>
	Vector4& operator[](T index) {
		return m_[index];
	}
	template<std::integral T>
	const Vector4& operator[](T index) const {
		return m_[index];
	}

	bool operator==(const Mat4x4& mat) const;
	bool operator!=(const Mat4x4& mat) const;

/// <summary>
/// メンバ関数
/// </summary>
public:
	const Mat4x4& Identity();

	/// <summary>
	/// 横ベクトル用平衡移動行列
	/// </summary>
	/// <param name="vec">移動</param>
	const Mat4x4& Translate(const Vector3& vec);

	/// <summary>
	/// スカラー倍行列
	/// </summary>
	/// <param name="vec">スカラー</param>
	const Mat4x4& Scalar(const Vector3& vec);

	/// <summary>
	/// 横ベクトル用X軸回転行列
	/// </summary>
	/// <param name="rad">ラジアン</param>
	const Mat4x4& RotateX(float rad);

	/// <summary>
	/// 横ベクトル用Y軸回転行列
	/// </summary>
	/// <param name="rad">ラジアン</param>
	const Mat4x4& RotateY(float rad);

	/// <summary>
	/// 横ベクトル用Z軸回転行列
	/// </summary>
	/// <param name="rad">ラジアン</param>
	const Mat4x4& RotateZ(float rad);

	/// <summary>
	/// 横ベクトル用アフィン行列
	/// </summary>
	/// <param name="scale">スカラー</param>
	/// <param name="rad">ラジアン</param>
	/// <param name="translate">移動</param>
	const Mat4x4& Affin(const Vector3& scale, const Vector3& rad, const Vector3& translate);

	/// <summary>
	/// 逆行列
	/// </summary>
	const Mat4x4& Inverse();

	/// <summary>
	///  転置行列
	/// </summary>
	const Mat4x4& Transepose();

	/// <summary>
	/// 横ベクトル用透視投影行列
	/// </summary>
	/// <param name="fovY">視野角</param>
	/// <param name="aspectRatio">アスペクト比</param>
	/// <param name="nearClip">ニアクリップ</param>
	/// <param name="farClip">ファークリップ</param>
	const Mat4x4& PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

	/// <summary>
	/// 横ベクトル用平衡投影行列
	/// </summary>
	/// <param name="left">左の範囲</param>
	/// <param name="right">右の範囲</param>
	/// <param name="top">上の範囲</param>
	/// <param name="bottom">下の範囲</param>
	/// <param name="nearClip">ニアクリップ</param>
	/// <param name="farClip">ファークリップ</param>
	const Mat4x4& Orthographic(float left, float right, float top, float bottom, float nearClip, float farClip);

	/// <summary>
	/// 横ベクトル用ビューポート行列
	/// </summary>
	/// <param name="left">映し出したViewPortの位置横</param>
	/// <param name="top">映し出したViewPortの位置縦</param>
	/// <param name="width">映し出す範囲横</param>
	/// <param name="height">映し出す範囲縦</param>
	/// <param name="minDepth">深度バッファーの最低値</param>
	/// <param name="maxDepth">深度バッファーの最大値</param>
	const Mat4x4& ViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);

	/// <summary>
	/// ストリング型を取得
	/// </summary>
	/// <returns></returns>
	std::string GetMatrixString();


/// <summary>
/// メンバ変数
/// </summary>
private:
	std::array<Vector4, 4> m_;

/// <summary>
/// 静的メンバ関数
/// </summary>
public:
	static Mat4x4 MakeTranslate(Vector3 vec);

	static Mat4x4 MakeScalar(Vector3 vec);

	static Mat4x4 MakeRotateX(float rad);

	static Mat4x4 MakeRotateY(float rad);

	static Mat4x4 MakeRotateZ(float rad);

	static Mat4x4 MakeRotate(const Vector3& rad);

	static Mat4x4 MakeAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);

	static Mat4x4 MakeAffin(const Vector3& scale, const Vector3& from, const Vector3& to, const Vector3& translate);

	static Mat4x4 MakeInverse(Mat4x4 mat);

	static Mat4x4 MakeTransepose(Mat4x4 mat);

	static Mat4x4 MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

	static Mat4x4 MakeOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip);

	static Mat4x4 MakeViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);


	static Mat4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

	static Mat4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

/// <summary>
/// 静的定数
/// </summary>
private:
	static constexpr int32_t kHeight_ = 4;
	static constexpr int32_t kWidth_ = 4;


/// <summary>
/// 静的定数
/// </summary>
public:
	static const Mat4x4 kIdentity;
	static const Mat4x4 kZero;
};