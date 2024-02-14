#pragma once

/// <summary>
/// 三次元配列
/// </summary>
class Vector3 final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Vector3() noexcept;
	Vector3(float x, float y, float z) noexcept;
	Vector3(const Vector3& right) noexcept;
	Vector3(const class Vector2& right) noexcept;
	Vector3(Vector3&& right) noexcept;
public:
	~Vector3() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	// 単項演算子

	Vector3 operator+() const noexcept;
	Vector3 operator-() const noexcept;


	// 二項演算子

	Vector3& operator=(const Vector3& right) noexcept;
	Vector3& operator=(Vector3&& right) noexcept;
	Vector3 operator+(const Vector3& right) const noexcept;
	Vector3 operator-(const Vector3& right) const noexcept;
	Vector3& operator+=(const Vector3& right) noexcept;
	Vector3& operator-=(const Vector3& right) noexcept;
	Vector3 operator*(float scalar) const noexcept;
	Vector3& operator*=(float scalar) noexcept;
	Vector3 operator/(float scalar) const noexcept;
	Vector3& operator/=(float scalar) noexcept;

	Vector3 operator*(const class Mat4x4& mat) const;
	Vector3& operator*=(const class Mat4x4& mat);
	friend Vector3 operator*(const class Mat4x4& left, const Vector3& right);

	Vector3& operator=(const class Vector2& right) noexcept;

	Vector3 operator*(const class Quaternion& right) const;
	Vector3& operator*=(const class Quaternion& right);


	// 等比演算子

	bool operator==(const Vector3& right) const noexcept;
	bool operator!=(const Vector3& right) const noexcept;

	// 
	float& operator[](size_t index);
	const float& operator[](size_t index) const;


/// <summary>
/// メンバ関数
/// </summary>
public:
	float Dot(const Vector3& right) const noexcept;
	Vector3 Cross(const Vector3& right) const noexcept;
	float Length() const noexcept;
	Vector3 Normalize() const noexcept;

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
	static Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

Vector3 Project(const Vector3& vec1, const Vector3& vec2);

Vector3 ClosestPoint(const Vector3& point, const Segment& segment);