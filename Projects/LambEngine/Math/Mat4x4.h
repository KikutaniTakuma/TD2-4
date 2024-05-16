#pragma once

#include <array>
#include <concepts>
#include <string>
#include "DirectXMath.h"
#include <initializer_list>
#include "Matrix.h"

#include "Error/Error.h"

// 4x4行列
template<>
class Matrix<float, 4, 4> final {
private:
	static constexpr size_t kWidth = 4;
	static constexpr size_t kHeight = 4;

	static_assert(0llu != kWidth and 0llu != kHeight,
		"Height and width must be greater than ""0""");
public:
	using value_type = float;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;


	using width_type = std::array<value_type, kWidth>;
	using width_pointer = width_type*;
	using width_const_pointer = const width_type*;
	using width_reference = width_type&;
	using width_const_reference = const width_type&;
	using height_type = std::array<width_type, kHeight>;

	using vector_type = std::array<value_type, kHeight* kWidth>;
	using vector_pointer = vector_type*;
	using vector_const_pointer = const vector_type*;
	using vector_reference = vector_type&;
	using vector_const_reference = const vector_type&;

	using matrix_type = height_type;
	using matrix_pointer = matrix_type*;
	using matrix_const_pointer = const matrix_type*;
	using matrix_reference = matrix_type&;
	using matrix_const_reference = const matrix_type&;



	using iterator = matrix_type::iterator;
	using const_iterator = matrix_type::const_iterator;

	using reverse_iterator = matrix_type::reverse_iterator;
	using const_reverse_iterator = matrix_type::const_reverse_iterator;

#ifndef value_cast
#define value_cast(num) static_cast<value_type>(num)
#endif // value_cast

public:
	constexpr Matrix() noexcept :
		vector_()
	{}

	constexpr Matrix(const vector_type& right) noexcept :
		vector_(right)
	{}
	constexpr Matrix(const Matrix&) = default;
	constexpr Matrix(Matrix&&) = default;
	constexpr Matrix(const DirectX::XMMATRIX& xmMatrix);


private:
	constexpr Matrix(value_type identity) noexcept
		: Matrix()
	{
		for (size_t i = 0; i < kHeight; i++) {
			matrix_[i][i] = identity;
		}
	}

public:
	~Matrix() = default;


#ifdef value_cast
public:
	constexpr Matrix& operator=(const Matrix&) = default;
	constexpr Matrix& operator=(Matrix&&) = default;

public:
	[[nodiscard]] constexpr width_reference operator[](size_t index) {
		if (kHeight <= index) {
			OutOfRange(ErrorPlace);
		}

		return matrix_[index];
	}

	[[nodiscard]] constexpr width_const_reference operator[](size_t index) const {
		if (kHeight <= index) {
			OutOfRange(ErrorPlace);
		}
		return matrix_[index];
	}

public:
	[[nodiscard]] Matrix operator*(const Matrix& right) const {
		Matrix result;

		result.xmMatrix_ = DirectX::XMMatrixMultiply(this->xmMatrix_, right.xmMatrix_);

		return result;
	}
	template<std::floating_point othertype, size_t otherHeight, size_t otherWidth>
	[[nodiscard]] constexpr Matrix<value_type, kHeight, otherWidth> operator*(const Matrix<othertype, otherHeight, otherWidth>& right) const requires(kWidth == otherHeight and kWidth != otherWidth) {
		Matrix<value_type, kHeight, otherWidth> result;

		for (size_t y = 0; y < result.HeightSize(); y++) {
			for (size_t x = 0; x < result.WidthSize(); x++) {
				for (size_t i = 0; i < kWidth; i++) {
					result[y][x] += matrix_[y][i] * value_cast(right[i][x]);
				}
			}
		}

		return result;
	}
	template<std::floating_point othertype, size_t otherHeight, size_t otherWidth>
	constexpr Matrix& operator*=(const Matrix<othertype, otherHeight, otherWidth>& right) requires(otherHeight == otherWidth and kWidth == otherHeight) {
		*this = *this * right;

		return *this;
	}

	[[nodiscard]] constexpr Matrix operator+(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t i = 0; i < size(); i++) {
			result.vector_[i] = this->vector_[i] + right.vector_[i];
		}

		return result;
	}
	constexpr Matrix& operator+=(const Matrix& right)noexcept {
		*this = *this + right;

		return *this;
	}
	[[nodiscard]] constexpr Matrix operator-(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t i = 0; i < size(); i++) {
			result.vector_[i] = this->vector_[i] - right.vector_[i];
		}

		return result;
	}
	constexpr Matrix& operator-=(const Matrix& right)noexcept {
		*this = *this - right;

		return *this;
	}
#endif // value_cast

public:
	[[nodiscard]] constexpr bool operator==(const Matrix& right) const {
		return vector_ == right.vector_;
	}
	[[nodiscard]] bool operator!=(const Matrix& right) const {
		return vector_ != right.vector_;
	}

public:
	[[nodiscard]] constexpr size_t HeightSize() const noexcept {
		return kHeight;
	}

	[[nodiscard]] constexpr size_t WidthSize() const noexcept {
		return kWidth;
	}

	[[nodiscard]] constexpr size_t size() const noexcept {
		return kHeight * kWidth;
	}
	[[nodiscard]] constexpr size_t max_size() const noexcept {
		return kHeight * kWidth;
	}
	[[nodiscard]] constexpr bool empty() const noexcept {
		return false;
	}

	[[nodiscard]] constexpr iterator begin() noexcept {
		return matrix_.begin();
	}
	[[nodiscard]] constexpr const_iterator cbegin() const noexcept {
		return matrix_.cbegin();
	}

	[[nodiscard]] constexpr iterator end() noexcept {
		return matrix_.end();
	}
	[[nodiscard]] constexpr const_iterator cend() const noexcept {
		return matrix_.cend();
	}

	[[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
		return matrix_.rbegin();
	}
	[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
		return matrix_.crbegin();
	}

	[[nodiscard]] constexpr reverse_iterator rend() noexcept {
		return matrix_.rend();
	}
	[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
		return matrix_.crend();
	}

	[[nodiscard]] constexpr width_reference at(size_t index) {
		if (kHeight <= index) {
			OutOfRange(ErrorPlace);
		}

		return matrix_.at(index);
	}

	[[nodiscard]] constexpr width_const_reference at(size_t index) const {
		if (kHeight <= index) {
			OutOfRange(ErrorPlace);
		}
		return matrix_.at(index);
	}

	[[nodiscard]] constexpr width_reference front() {
		return matrix_.front();
	}
	[[nodiscard]] constexpr width_const_reference front() const {
		return matrix_.front();
	}
	[[nodiscard]] constexpr width_reference back() {
		return matrix_.back();
	}
	[[nodiscard]] constexpr width_const_reference back() const {
		return matrix_.back();
	}

	[[nodiscard]] pointer data() noexcept {
		return vector_.data();
	}

	[[nodiscard]] const_pointer data() const noexcept {
		return vector_.data();
	}

	[[nodiscard]] constexpr vector_reference GetVector() noexcept {
		return vector_;
	}
	[[nodiscard]] constexpr vector_const_reference GetVector() const noexcept {
		return vector_;
	}

	[[nodiscard]] constexpr matrix_reference GetMatrix() noexcept {
		return matrix_;
	}
	[[nodiscard]] constexpr matrix_const_reference GetMatrix() const noexcept {
		return matrix_;
	}
	[[nodiscard]] constexpr DirectX::XMMATRIX& GetXMMatrix() noexcept {
		return xmMatrix_;
	}
	[[nodiscard]] constexpr const DirectX::XMMATRIX& GetXMMatrix() const noexcept {
		return xmMatrix_;
	}

	constexpr void swap(Matrix& right) {
		vector_.swap(right.vector_);
	}

	constexpr void fill(value_type value) {
		vector_.fill(value);
	}


public:
	static [[nodiscard]] const Matrix& Identity() {
		return kIdentity;
	}

public:
	[[nodiscard]] Matrix Inverse() const {
		Matrix result;
		DirectX::XMVECTOR det;
		result.xmMatrix_ = DirectX::XMMatrixInverse(&det, this->xmMatrix_);

		return result;
	}

	[[nodiscard]] Matrix Transepose() const {
		Matrix result;

		result.xmMatrix_ = DirectX::XMMatrixTranspose(this->xmMatrix_);

		return result;
	}

	/// <summary>
	/// メンバ関数
	/// </summary>
public:
	[[nodiscard]] std::string GetString() const {
		std::string str;

		for (auto& line : this->matrix_) {
			for (auto& i : line) {
				str += std::to_string(i) + ", ";
			}
			str += "\n";
		}

		return str;
	}

private:
	void OutOfRange(const std::string& funcName,
		const std::string& sourceFileName,
		uint32_t codeLineNumber) const
	{
		throw Lamb::Error::Code<Matrix>("out of range", funcName, sourceFileName, codeLineNumber);
	}

	/// <summary>
	/// メンバ変数
	/// </summary>
protected:
	union {
		matrix_type matrix_;
		vector_type vector_;
		DirectX::XMMATRIX xmMatrix_;
	};

	/// <summary>
	/// 4x4行列用の関数
	/// </summary>
public:
	class Vector3 GetTranslate() const;
	class Vector3 GetScale() const;
	class Quaternion GetRotate() const;

	void Decompose(class Vector3& scale, class Quaternion& rotate, class Vector3& translate) const;
	void Decompose(class Vector3& scale, class Vector3& rotate, class Vector3& translate) const;

	// 静的メンバ関数
public:
	static [[nodiscard]] Matrix MakeTranslate(const class Vector3& vec);

	static [[nodiscard]] Matrix MakeScalar(const class Vector3& vec);

	static [[nodiscard]] Matrix MakeRotateX(float rad);

	static [[nodiscard]] Matrix MakeRotateY(float rad);

	static [[nodiscard]] Matrix MakeRotateZ(float rad);

	static [[nodiscard]] Matrix MakeRotate(const class Vector3& rad);

	static [[nodiscard]] Matrix MakeRotate(const class Quaternion& rad);

	static [[nodiscard]] Matrix MakeAffin(const class Vector3& scale, const class Vector3& rad, const class Vector3& translate);

	static [[nodiscard]] Matrix MakeAffin(const class Vector3& scale, const class Quaternion& rad, const class Vector3& translate);

	static [[nodiscard]] Matrix MakeAffin(const class Vector3& scale, const class Vector3& from, const class Vector3& to, const class Vector3& translate);

	static [[nodiscard]] Matrix MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

	static [[nodiscard]] Matrix MakeOrthographic(float width, float height, float nearClip, float farClip);

	static [[nodiscard]] Matrix MakeViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);


	static [[nodiscard]] Matrix DirectionToDirection(const class Vector3& from, const class Vector3& to);

	static [[nodiscard]] Matrix MakeRotateAxisAngle(const class Vector3& axis, float angle);

	// 静的定数
public:
	static const Matrix kIdentity;
	static const Matrix kZero;
};

namespace {
	using Mat4x4 = Matrix<float, 4, 4>;
}