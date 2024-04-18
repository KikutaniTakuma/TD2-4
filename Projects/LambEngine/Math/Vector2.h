#pragma once

/// <summary>
///	二次元ベクトル
/// </summary>
class Vector2 final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	constexpr Vector2() noexcept;
	Vector2(float x, float y) noexcept;

	constexpr Vector2(const Vector2& right) = default;
	constexpr Vector2(Vector2&& right) = default;
public:
	~Vector2() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	[[nodiscard]] inline Vector2 operator+() const noexcept {
		return *this;
	}
	[[nodiscard]] inline Vector2 operator-() const noexcept {
		return { -x, -y };
	}

	[[nodiscard]] Vector2 operator+(const Vector2& right) const noexcept;
	[[nodiscard]] Vector2 operator-(const Vector2& right) const noexcept;
	[[nodiscard]] Vector2 operator*(float scalar) const noexcept;
	[[nodiscard]] Vector2 operator/(float scalar) const noexcept;
	Vector2& operator=(const Vector2& right) noexcept;
	Vector2& operator=(Vector2&& right) noexcept;
	Vector2& operator+=(const Vector2& right) noexcept;
	Vector2& operator-=(const Vector2& right) noexcept;
	Vector2& operator*=(float scalar) noexcept;
	Vector2& operator/=(float scalar) noexcept;
	[[nodiscard]] bool operator==(const Vector2& right) const noexcept;
	[[nodiscard]] bool operator!=(const Vector2& right) const noexcept;

/// <summary>
/// メンバ関数
/// </summary>
public:
	[[noreturn]] void Rotate(float rad) noexcept;

	[[nodiscard]] float Cross(const Vector2& right) const noexcept;
	[[nodiscard]] float Dot(const Vector2& right) const noexcept;
	[[nodiscard]] float Length() const noexcept;
	[[nodiscard]] float LengthSQ() const noexcept;

	[[nodiscard]] Vector2 Normalize() const noexcept;

	[[nodiscard]] float GetRad() const noexcept;

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
	static [[nodiscard]] Vector2 Lerp(const Vector2& start, const Vector2& end, float easeSpd);

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