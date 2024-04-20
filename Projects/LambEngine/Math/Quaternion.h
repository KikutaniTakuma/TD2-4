#pragma once
#include "Vector3.h"
#include "Vector4.h"

using LogQuaternion = Vector3;

/// <summary>
/// クォータニオンクラス
/// </summary>
class Quaternion final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	constexpr Quaternion();
	constexpr Quaternion(const Quaternion&) = default;
	constexpr Quaternion(Quaternion&&) = default;
	Quaternion(const Vector4& right);
	Quaternion(const Vector3& right, float w);
	Quaternion(const std::array<float, 4>& right);
	Quaternion(float x, float y, float z, float w);
	~Quaternion() = default;

/// <summary>
/// 単項演算子
/// </summary>
public:
	[[nodiscard]] Quaternion operator+() const noexcept;
	[[nodiscard]] Quaternion operator-() const noexcept;

/// <summary>
/// 二項演算子のオーバーロード
/// </summary>
public:
	Quaternion& operator=(const Quaternion&) = default;
	Quaternion& operator=(Quaternion&&)noexcept = default;

	Quaternion& operator=(const Vector4& right);

	[[nodiscard]] Quaternion operator*(const Quaternion& right) const;
	Quaternion& operator*=(const Quaternion& right);

	[[nodiscard]] Quaternion operator+(const Quaternion& right) const;
	Quaternion& operator+=(const Quaternion& right);

	[[nodiscard]] Quaternion operator-(const Quaternion& right) const;
	Quaternion& operator-=(const Quaternion& right);

	[[nodiscard]] Quaternion operator*(float right) const;
	friend Quaternion operator*(float right, const Quaternion& left);
	Quaternion& operator*=(float right);

	[[nodiscard]] Quaternion operator/(float right) const;
	friend Quaternion operator/(float right, const Quaternion& left);
	Quaternion& operator/=(float right);

	[[nodiscard]] bool operator==(const Quaternion& right) const;
	[[nodiscard]] bool operator!=(const Quaternion& right) const;

/// <summary>
/// 配列関係の関数
/// </summary>
public:
	[[nodiscard]] float* data() noexcept {
		return m.data();
	}

	[[nodiscard]] const float* data() const noexcept {
		return m.data();
	}

	[[nodiscard]] std::array<float, 4>::iterator begin() noexcept {
		return m.begin();
	}
	[[nodiscard]] std::array<float, 4>::iterator end() noexcept {
		return m.end();
	}
	[[nodiscard]] std::array<float, 4>::const_iterator cbegin() const noexcept {
		return m.cbegin();
	}
	[[nodiscard]] std::array<float, 4>::const_iterator cend() const noexcept {
		return m.cend();
	}
	[[nodiscard]] std::array<float, 4>::reverse_iterator rbegin() noexcept {
		return m.rbegin();
	}
	[[nodiscard]] std::array<float, 4>::reverse_iterator rend() noexcept {
		return m.rend();
	}
	[[nodiscard]] std::array<float, 4>::const_reverse_iterator crbegin() const noexcept {
		return m.crbegin();
	}
	[[nodiscard]] std::array<float, 4>::const_reverse_iterator crend() const noexcept {
		return m.crend();
	}

public:
	/// <summary>
	/// 共役
	/// </summary>
	/// <returns>このクォータニオンの虚部を反転させたもの</returns>
	[[nodiscard]] Quaternion Conjugate() const;

	/// <summary>
	/// クォータニオンを4次元ベクトルとしての内積
	/// </summary>
	/// <param name="other">他のクォータニオン</param>
	/// <returns>内積結果</returns>
	[[nodiscard]] float Dot(const Quaternion& other) const;

	/// <summary>
	/// 長さ取得
	/// </summary>
	/// <returns>ノルム</returns>
	[[nodiscard]] float Length() const;

	/// <summary>
	/// 長さ1のクォータニオンを返す
	/// </summary>
	/// <returns>単位クォータニオン</returns>
	[[nodiscard]] Quaternion Normalize() const;

	/// <summary>
	/// 逆クォータニオンを返す
	/// </summary>
	/// <returns>逆クォータニオン</returns>
	[[nodiscard]] Quaternion Inverce() const;

	[[nodiscard]] Vector3 GetDirectionX() const;

	[[nodiscard]] Vector3 GetDirectionY() const;

	[[nodiscard]] Vector3 GetDirectionZ() const;


	/// <summary>
	/// 回転行列を取得
	/// </summary>
	/// <returns>クォータニオンからの回転行列</returns>
	[[nodiscard]] class Mat4x4 GetMatrix() const;



/// <summary>
/// 静的メンバ関数
/// </summary>
public:
	/// <summary>
	/// クォータニオン版のDirectionToDirection
	/// </summary>
	/// <param name="from">始点</param>
	/// <param name="to">終点</param>
	/// <returns>クォータニオン</returns>
	static [[nodiscard]] Quaternion DirectionToDirection(const Vector3& from, const Vector3& to);

	/// <summary>
	/// 任意軸回転
	/// </summary>
	/// <param name="axis">任意軸の方向ベクトル(単位ベクトル)</param>
	/// <param name="angle">任意軸での回転量</param>
	/// <returns>任意軸回転を適用したクォータニオン</returns>
	static [[nodiscard]] Quaternion MakeRotateAxisAngle(const Vector3& axis, float angle);
	/// <summary>
	/// x軸回転クォータニオン
	/// </summary>
	/// <param name="angle">オイラー角</param>
	/// <returns>x軸回転を適用したクォータニオン</returns>
	static [[nodiscard]] Quaternion MakeRotateXAxis(float angle);
	// <summary>
	/// y軸回転クォータニオン
	/// </summary>
	/// <param name="angle">オイラー角</param>
	/// <returns>y軸回転を適用したクォータニオン</returns>
	static [[nodiscard]] Quaternion MakeRotateYAxis(float angle);
	// <summary>
	/// z軸回転クォータニオン
	/// </summary>
	/// <param name="angle">オイラー角</param>
	/// <returns>z軸回転を適用したクォータニオン</returns>
	static [[nodiscard]] Quaternion MakeRotateZAxis(float angle);

	/// <summary>
	/// オイラー角からQuaternion
	/// </summary>
	/// <param name="euler">オイラー角</param>
	/// <returns>Quaternion</returns>
	static [[nodiscard]] Quaternion EulerToQuaternion(const Vector3& euler);

	/// <summary>
	/// クォータニオン線形補完関数(近いものの方向に回転する)
	/// </summary>
	/// <param name="start">スタート時の回転</param>
	/// <param name="end">終わりの回転</param>
	/// <param name="t">0.0f～1.0f</param>
	/// <returns>補完されたクォータニオン</returns>
	static [[nodiscard]] Quaternion Slerp(Quaternion start, const Quaternion& end, float t);

	/// <summary>
	/// 対数Quaternionへの変換
	/// </summary>
	/// <param name="quaternion">変換するQuaternion</param>
	/// <returns>対数Quaternion</returns>
	static [[nodiscard]] LogQuaternion Log(const Quaternion& quaternion);
	/// <summary>
	/// 対数QuaternionからQuaternionへの変換
	/// </summary>
	/// <param name="logQuaternion">対数Quaternion</param>
	/// <returns>Quaternion</returns>
	static [[nodiscard]] Quaternion Exp(const LogQuaternion& logQuaternion);


/// <summary>
/// 静的メンバ定数
/// </summary>
public:
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f w = 1.0f
	/// </summary>
	static const Quaternion kIdentity;

	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f w = 0.0f
	/// </summary>
	static const Quaternion kZero;

/// <summary>
/// メンバ変数
/// </summary>
public:
	union
	{
		std::array<float, 4> m;
		__m128 m128;
		Vector4 vector4;
		struct {
			Vector3 vector3;
			float w;
		}vector;

		struct {
			float x;
			float y;
			float z;
			float w;
		}quaternion;
	};
};