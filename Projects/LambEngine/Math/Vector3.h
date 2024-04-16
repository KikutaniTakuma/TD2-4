#pragma once
#include <iterator>

/// <summary>
/// 三次元配列
/// </summary>
class Vector3 final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	constexpr Vector3() noexcept;
	Vector3(float x, float y, float z) noexcept;
	constexpr Vector3(const Vector3&) noexcept = default;
	Vector3(const class Vector2& right) noexcept;
	constexpr Vector3(Vector3&&) noexcept = default;
public:
	~Vector3() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	// 単項演算子

	[[nodiscard]] Vector3 operator+() const noexcept;
	[[nodiscard]] Vector3 operator-() const noexcept;


	// 二項演算子

	Vector3& operator=(const Vector3&) = default;
	Vector3& operator=(Vector3&&) = default;
	[[nodiscard]] Vector3 operator+(const Vector3& right) const noexcept;
	[[nodiscard]] Vector3 operator-(const Vector3& right) const noexcept;
	Vector3& operator+=(const Vector3& right) noexcept;
	Vector3& operator-=(const Vector3& right) noexcept;
	[[nodiscard]] Vector3 operator*(float scalar) const noexcept;
	Vector3& operator*=(float scalar) noexcept;
	[[nodiscard]] Vector3 operator/(float scalar) const noexcept;
	Vector3& operator/=(float scalar) noexcept;

	[[nodiscard]] Vector3 operator*(const class Mat4x4& mat) const;
	Vector3& operator*=(const class Mat4x4& mat);
	friend Vector3 operator*(const class Mat4x4& left, const Vector3& right);

	Vector3& operator=(const class Vector2& right) noexcept;

	[[nodiscard]] Vector3 operator*(const class Quaternion& right) const;
	Vector3& operator*=(const class Quaternion& right);


	// 等比演算子

	[[nodiscard]] bool operator==(const Vector3& right) const noexcept;
	[[nodiscard]] bool operator!=(const Vector3& right) const noexcept;

	// 
	[[nodiscard]] float& operator[](size_t index);
	[[nodiscard]] const float& operator[](size_t index) const;


/// <summary>
/// メンバ関数
/// </summary>
public:
	[[nodiscard]] float Dot(const Vector3& right) const noexcept;
	[[nodiscard]] Vector3 Cross(const Vector3& right) const noexcept;
	[[nodiscard]] float Length() const noexcept;
	[[nodiscard]] float LengthSQ() const noexcept;
	[[nodiscard]] Vector3 Normalize() const noexcept;
	[[nodiscard]] float* data() noexcept {
		return &x;
	}
	[[nodiscard]] const float* data() const noexcept {
		return &x;
	}
	[[nodiscard]] auto begin() noexcept {
		return std::data(*this);
	}
	[[nodiscard]] auto end() noexcept {
		return std::data(*this) + 3;
	}
	[[nodiscard]] auto cbegin() const noexcept {
		return std::data(*this);
	}
	[[nodiscard]] auto cend() const noexcept {
		return std::data(*this) + 3;
	}
	[[nodiscard]] auto rbegin() noexcept {
		return std::make_reverse_iterator(end());
	}
	[[nodiscard]] auto rend() noexcept {
		return std::make_reverse_iterator(begin());
	}
	[[nodiscard]] auto crbegin() const noexcept {
		return std::make_reverse_iterator(cend());
	}
	[[nodiscard]] auto crend() const noexcept {
		return std::make_reverse_iterator(cbegin());
	}


/// <summary>
/// 静的定数
/// </summary>
public:
	/// <summary>
	/// x = 1.0f, y = 1.0f, z = 1.0f
	/// </summary>
	static const Vector3 kIdentity;
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f
	/// </summary>
	static const Vector3 kZero;
	/// <summary>
	/// x = 1.0f, y = 0.0f, z = 0.0f
	/// </summary>
	static const Vector3 kXIdentity;
	/// <summary>
	/// x = 0.0f, y = 1.0f, z = 0.0f
	/// </summary>
	static const Vector3 kYIdentity;
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 1.0f
	/// </summary>
	static const Vector3 kZIdentity;

/// <summary>
/// メンバ変数
/// </summary>
public:
	float x;
	float y;
	float z;


/// <summary>
/// 静的関数
/// </summary>
public:
	static [[nodiscard]] Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
	
	static [[nodiscard]] class Vector3 QuaternionToEuler(const Quaternion& quaternion);
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

[[nodiscard]] Vector3 Project(const Vector3& vec1, const Vector3& vec2);

[[nodiscard]] Vector3 ClosestPoint(const Vector3& point, const Segment& segment);