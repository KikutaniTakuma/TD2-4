#pragma once
#include <array>

/// <summary>
///	二次元ベクトル
/// </summary>
class Vector2 final {
private:
	static constexpr size_t arraySize = 2llu;

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

	[[nodiscard]] float& operator[](size_t index);
	[[nodiscard]] const float& operator[](size_t index) const;

/// <summary>
/// メンバ関数
/// </summary>
public:
	void Rotate(float rad) noexcept;

	[[nodiscard]] float Cross(const Vector2& right) const noexcept;
	[[nodiscard]] float Dot(const Vector2& right) const noexcept;
	[[nodiscard]] float Length() const noexcept;
	[[nodiscard]] float LengthSQ() const noexcept;

	[[nodiscard]] Vector2 Normalize() const noexcept;

	[[nodiscard]] float GetRad() const noexcept;

public:
	constexpr void fill(float value) {
		std::fill_n(data(), size(), value);
	}
	constexpr void swap(Vector2& other)noexcept(std::_Is_nothrow_swappable<float>::value) {
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
		return y;
	}
	[[nodiscard]] constexpr const float& back() const {
		return y;
	}
	[[nodiscard]] float& at(size_t index);
	[[nodiscard]] const float& at(size_t index) const;



/// <summary>
/// メンバ変数
/// </summary>
public:
	union {
		float x;
		float first;
		float min;
	};
	union {
		float y;
		float second;
		float max;
	};

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