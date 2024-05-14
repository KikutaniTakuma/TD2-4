#pragma once
#include <array>
#include <immintrin.h>
#include "Utils/Cocepts/Cocepts.h"
#include "Mat4x4.h"

/// <summary>
/// 4次元配列
/// </summary>
class Vector4 final {
private:
	static constexpr size_t arraySize = 4llu;

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
	constexpr Vector4() noexcept;
	constexpr Vector4(const Vector4&) = default;
	constexpr Vector4(Vector4&&) = default;
	Vector4(float x, float y, float z, float w) noexcept;
	Vector4(const class Vector3& vec3, float w) noexcept;
	Vector4(const class Vector2& vec2, float z, float w) noexcept;
	Vector4(uint32_t right) noexcept;
	Vector4(const std::array<float, 4>& right) noexcept;
public:
	~Vector4() = default;

	/// <summary>
	/// 演算子のオーバーロード
	/// </summary>
public:
	[[nodiscard]] Vector4 operator+() const noexcept;
	[[nodiscard]] Vector4 operator-() const noexcept;

	Vector4& operator=(const Vector4& right) noexcept = default;
	Vector4& operator=(Vector4&& right) noexcept = default;
	Vector4& operator=(const class Vector3& right) noexcept;
	Vector4& operator=(const class Vector2& right) noexcept;
	Vector4& operator=(uint32_t right) noexcept;

	[[nodiscard]] Vector4 operator+(const Vector4& right) const noexcept;
	Vector4& operator+=(const Vector4& right) noexcept;

	[[nodiscard]] Vector4 operator-(const Vector4& right) const noexcept;
	Vector4& operator-=(const Vector4& right) noexcept;

	[[nodiscard]] Vector4 operator*(const Vector4& right) const noexcept;
	Vector4& operator*=(const Vector4& right) noexcept;
	[[nodiscard]] Vector4 operator/(const Vector4& right) const noexcept;
	Vector4& operator/=(const Vector4& right) noexcept;
	[[nodiscard]] Vector4 operator*(float scalar) const noexcept;
	Vector4& operator*=(float scalar) noexcept;
	[[nodiscard]] Vector4 operator/(float scalar) const noexcept;
	Vector4& operator/=(float scalar) noexcept;

	[[nodiscard]] Vector4 operator*(const Mat4x4& mat) const noexcept;
	Vector4& operator*=(const Mat4x4& mat) noexcept;

	[[nodiscard]] bool operator==(const Vector4& right) const noexcept;
	[[nodiscard]] bool operator!=(const Vector4& right) const noexcept;

	[[nodiscard]] float& operator[](size_t index);
	[[nodiscard]] const float& operator[](size_t index) const;

	/// <summary>
	/// メンバ関数
	/// </summary>
public:
	[[nodiscard]] float Length() const noexcept;
	[[nodiscard]] float LengthSQ() const noexcept;

	[[nodiscard]] Vector4 Normalize() const noexcept;

	[[nodiscard]] float Dot(const Vector4& right) const noexcept;

	[[nodiscard]] class Vector3 GetVector3() const noexcept;
	[[nodiscard]] class Vector2 GetVector2() const noexcept;


	[[nodiscard]] uint32_t GetColorRGBA() const;

/// <summary>
/// 配列関係の関数
/// </summary>
public:
	constexpr void fill(float value) {
		std::fill_n(data(), size(), value);
	}
	constexpr void swap(Vector4& other)noexcept(std::_Is_nothrow_swappable<float>::value) {
		std::_Swap_ranges_unchecked(data(), data() + size(), other.data());
	}

	[[nodiscard]] float* data() noexcept {
		return m.data();
	}
	[[nodiscard]] const float* data() const noexcept {
		return m.data();
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
		return m.front();
	}
	[[nodiscard]] constexpr const float& front() const {
		return m.front();
	}
	[[nodiscard]] constexpr float& back() {
		return m.back();
	}
	[[nodiscard]] constexpr const float& back() const {
		return m.back();
	}
	[[nodiscard]] float& at(size_t index);
	[[nodiscard]] const float& at(size_t index) const;


/// <summary>
/// 静的定数
/// </summary>
public:
	/// <summary>
	/// x = 1.0f, y = 1.0f, z = 1.0f, w = 1.0f
	/// </summary>
	static const Vector4 kIdentity;
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f
	/// </summary>
	static const Vector4 kZero;
	/// <summary>
	/// x = 1.0f, y = 0.0f, z = 0.0f, w = 0.0f
	/// </summary>
	static const Vector4 kXIdentity;
	/// <summary>
	/// x = 0.0f, y = 1.0f, z = 0.0f, w = 0.0f
	/// </summary>
	static const Vector4 kYIdentity;
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 1.0f, w = 0.0f
	/// </summary>
	static const Vector4 kZIdentity;
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f
	/// </summary>
	static const Vector4 kWIdentity;


	/// <summary>
	/// メンバ変数
	/// </summary>
public:
	union {
		std::array<float, arraySize> m;
		struct {
			float x;
			float y;
			float z;
			float w;
		} vec;
		struct {
			float r;
			float g;
			float b;
			float a;
		} color;
		__m128 m128;
	};

};

/// <summary>
/// uint32_tからVector4への変換
/// </summary>
[[nodiscard]] Vector4 UintToVector4(uint32_t color);

/// <summary>
/// Vector4からuint32_tへの変換
/// </summary>
[[nodiscard]] uint32_t Vector4ToUint(const Vector4& color);

[[nodiscard]] Vector4 ColorLerp(const Vector4& start, const Vector4& end, float t);

[[nodiscard]] uint32_t ColorLerp(uint32_t start, uint32_t end, float t);