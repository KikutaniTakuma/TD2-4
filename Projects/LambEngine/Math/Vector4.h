#pragma once
#include <array>
#include <immintrin.h>
#include "Utils/Cocepts/Cocepts.h"

/// <summary>
/// 4次元配列
/// </summary>
class Vector4 final {
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

	[[nodiscard]] Vector4 operator*(float scalar) const noexcept;
	Vector4& operator*=(float scalar) noexcept;
	[[nodiscard]] Vector4 operator/(float scalar) const noexcept;
	Vector4& operator/=(float scalar) noexcept;

	[[nodiscard]] Vector4 operator*(const class Mat4x4& mat) const noexcept;
	Vector4& operator*=(const class Mat4x4& mat) noexcept;
	friend Vector4 operator*(const class Mat4x4& left, const Vector4& right) noexcept;

	[[nodiscard]] bool operator==(const Vector4& right) const noexcept;
	[[nodiscard]] bool operator!=(const Vector4& right) const noexcept;

	template<Lamb::IsInt T>
	[[nodiscard]] float& operator[](T index) noexcept {
		return m[index];
	}
	template<Lamb::IsInt T>
	[[nodiscard]] const float& operator[](T index) const noexcept {
		return m[index];
	}

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
	[[nodiscard]] float& front() noexcept {
		return m.front();
	}
	[[nodiscard]] const float& front() const noexcept {
		return m.front();
	}
	[[nodiscard]] float& back() noexcept {
		return m.back();
	}
	[[nodiscard]] const float& back() const noexcept {
		return m.back();
	}
	[[nodiscard]] float& at(size_t index) noexcept {
		return m.at(index);
	}
	[[nodiscard]] const float& at(size_t index) const noexcept {
		return m.at(index);
	}

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
		std::array<float, 4> m;
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