#pragma once

/// <summary>
///	二次元ベクトル
/// </summary>
class Vector2 final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Vector2() noexcept;
	Vector2(float x, float y) noexcept;

	Vector2(const Vector2& right) noexcept;
	Vector2(Vector2&& right) noexcept;
public:
	~Vector2() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	inline Vector2 operator+() const noexcept {
		return *this;
	}
	inline Vector2 operator-() const noexcept {
		return { -x, -y };
	}

	Vector2 operator+(const Vector2& right) const noexcept;
	Vector2 operator-(const Vector2& right) const noexcept;
	Vector2 operator*(float scalar) const noexcept;
	Vector2 operator/(float scalar) const noexcept;
	Vector2& operator=(const Vector2& right) noexcept;
	Vector2& operator=(Vector2&& right) noexcept;
	Vector2& operator+=(const Vector2& right) noexcept;
	Vector2& operator-=(const Vector2& right) noexcept;
	Vector2& operator*=(float scalar) noexcept;
	Vector2& operator/=(float scalar) noexcept;
	bool operator==(const Vector2& right) const noexcept;
	bool operator!=(const Vector2& right) const noexcept;

/// <summary>
/// メンバ関数
/// </summary>
public:
	void Rotate(float rad) noexcept;

	float Cross(const Vector2& right) const noexcept;
	float Dot(const Vector2& right) const noexcept;
	float Length() const noexcept;

	Vector2 Normalize() const noexcept;

	float GetRad() const noexcept;

/// <summary>
/// メンバ変数
/// </summary>
public:
	float x;
	float y;

/// <summary>
/// 静的関数
/// </summary>
public:
	static Vector2 Lerp(const Vector2& start, const Vector2& end, float easeSpd);

/// <summary>
/// 静的定数
/// </summary>
public:
	/// <summary>
	/// x = 1.0f, y = 1.0f
	/// </summary>
	static const Vector2 kIdentity;
	/// <summary>
	/// x = 0.0f, y = 0.0f
	/// </summary>
	static const Vector2 kZero;
	/// <summary>
	/// x = 1.0f, y = 0.0f
	/// </summary>
	static const Vector2 kXIdentity;
	/// <summary>
	/// x = 0.0f, y = 1.0f
	/// </summary>
	static const Vector2 kYIdentity;

};