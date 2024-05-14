#pragma once
#include <array>
#include "Mat4x4.h"

/// <summary>
/// 三次元配列
/// </summary>
class Vector3 final {
private:
	static constexpr size_t arraySize = 3llu;

public:
	using size_type = size_t;

	using iterator = std::_Array_iterator<float, arraySize>;
	using const_iterator = std::_Array_const_iterator<float, arraySize>;

	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;


/// <summary>
/// コンストラクタ
/// </summary>
public:
	constexpr Vector3() noexcept;
	Vector3(float x, float y, float z) noexcept;
	constexpr Vector3(const Vector3&) noexcept = default;
	Vector3(const class Vector2& right) noexcept;
	Vector3(const class Vector2& right, float rightZ) noexcept;
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

	[[nodiscard]] Vector3 operator*(const Mat4x4& mat) const;
	Vector3& operator*=(const Mat4x4& mat);

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

public:
	constexpr void fill(float value) {
		std::fill_n(data(), size(), value);
	}
	constexpr void swap(Vector3& other)noexcept(std::_Is_nothrow_swappable<float>::value) {
		std::_Swap_ranges_unchecked(data(), data() + size(), other.data());
	}

	[[nodiscard]] float* data() noexcept {
		return &x;
	}
	[[nodiscard]] const float* data() const noexcept {
		return &x;
	}
	[[nodiscard]] constexpr iterator begin() noexcept {
		return iterator(data(), 0);
	}

	[[nodiscard]] constexpr const_iterator begin() const noexcept {
		return const_iterator(data(), 0);
	}

	[[nodiscard]] constexpr iterator end() noexcept {
		return iterator(data(), size());
	}

	[[nodiscard]] constexpr const_iterator end() const noexcept {
		return const_iterator(data(), size());
	}

	[[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	[[nodiscard]] constexpr reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	[[nodiscard]] constexpr const_iterator cbegin() const noexcept {
		return begin();
	}

	[[nodiscard]] constexpr const_iterator cend() const noexcept {
		return end();
	}

	[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
		return rend();
	}

	[[nodiscard]] constexpr size_type size() const {
		return arraySize;
	}
	[[nodiscard]] constexpr size_type max_size() const {
		return arraySize;
	}

	[[nodiscard]] constexpr bool empty() const {
		return false;
	}

	[[nodiscard]] constexpr float& front() {
		return x;
	}
	[[nodiscard]] constexpr const float& front() const {
		return x;
	}
	[[nodiscard]] constexpr float& back() {
		return z;
	}
	[[nodiscard]] constexpr const float& back() const {
		return z;
	}
	[[nodiscard]] float& at(size_t index);
	[[nodiscard]] const float& at(size_t index) const;



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
	union {
		float x;
		float r;
	};
	union {
		float y;
		float g;
	};
	union {
		float z;
		float b;
	};


/// <summary>
/// 静的関数
/// </summary>
public:
	static [[nodiscard]] Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
	
	static [[nodiscard]] Vector3 QuaternionToEuler(const class Quaternion& quaternion);
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

/// <summary>
/// ベクトル1をベクトル2に投影する関数
/// </summary>
/// <param name="vec1">投影するベクトル</param>
/// <param name="vec2">投影先のベクトル</param>
/// <returns>正射影ベクトル</returns>
[[nodiscard]] Vector3 Project(const Vector3& vec1, const Vector3& vec2);

[[nodiscard]] Vector3 ClosestPoint(const Vector3& point, const Segment& segment);