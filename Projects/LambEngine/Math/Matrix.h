#pragma once
#include <array>
#include <type_traits>
#include <string>

#include "Error/Error.h"

template<std::floating_point floatingType, size_t height, size_t width>
class Matrix {
public:
	using ValueType = floatingType;

	using WidthType = std::array<ValueType, width>;
	using HeightType = std::array<WidthType, height>;

	using VectorType = std::array<ValueType, height * width>;
	using MatrixType = HeightType;

	using iterator = MatrixType::iterator;
	using const_iterator = MatrixType::const_iterator;

	using reverse_iterator = MatrixType::reverse_iterator;
	using const_reverse_iterator = MatrixType::const_reverse_iterator;

public:
	constexpr Matrix() noexcept :
		vector_()
	{}

	constexpr Matrix(const VectorType& num) noexcept : 
		vector_(num)
	{}
	constexpr Matrix(const Matrix&) = default;
	constexpr Matrix(Matrix&&) = default;
	~Matrix() = default;

public:
	Matrix& operator=(const Matrix&) = default;
	Matrix& operator=(Matrix&&) = default;

public:
	template<std::integral T>
	[[nodiscard]] constexpr WidthType& operator[](T index) {
		if (matrix_.size() <= index) {
			throw Lamb::Error::Code<Matrix>("out of range", __func__);
		}

		return matrix_[index];
	}

	template<std::integral T>
	[[nodiscard]] constexpr const WidthType& operator[](T index) const {
		if (matrix_.size() <= index) {
			throw Lamb::Error::Code<Matrix>("out of range", __func__);
		}
		return matrix_[index];
	}

public:
	template<size_t otherWidth>
	[[nodiscard]] Matrix<ValueType, height, otherWidth> operator*(const Matrix<ValueType, width, otherWidth>& right) const noexcept {
		Matrix<ValueType, height, otherWidth> result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < otherWidth; x++) {
				for (size_t i = 0; i < width; i++) {
					result[y][x] += matrix_[y][i] * right[i][x];
				}
			}
		}

		return result;
	}
	template<size_t otherWidth>
	Matrix<ValueType, height, otherWidth>& operator*=(const Matrix<ValueType, height, otherWidth>& right)noexcept {
		*this = *this * right;

		return *this;
	}

	[[nodiscard]] Matrix operator+(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				result[y][x] = matrix_[y][x] + right[y][x];
			}
		}

		return result;
	}
	Matrix& operator+=(const Matrix& right)noexcept {
		*this = *this + right;

		return *this;
	}
	[[nodiscard]] Matrix operator-(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				result[y][x] = matrix_[y][x] - right[y][x];
			}
		}

		return result;
	}
	Matrix& operator-=(const Matrix& right)noexcept {
		*this = *this - right;

		return *this;
	}

public:
	[[nodiscard]] bool operator==(const Matrix& right) const {
		return vector_ == right.vector_;
	}
	[[nodiscard]] bool operator!=(const Matrix& right) const {
		return vector_ != right.vector_;
	}

public:
	[[nodiscard]] constexpr size_t HeightSize() const noexcept {
		return height;
	}

	[[nodiscard]] constexpr size_t WidthSize() const noexcept {
		return width;
	}

	[[nodiscard]] constexpr size_t size() const noexcept {
		return height * width;
	}

	[[nodiscard]] constexpr iterator begin() noexcept {
		return matrix_.begin();
	}
	[[nodiscard]] constexpr const_iterator begin() const noexcept {
		return matrix_.begin();
	}

	[[nodiscard]] constexpr iterator end() noexcept {
		return matrix_.end();
	}
	[[nodiscard]] constexpr const_iterator end() const noexcept {
		return matrix_.end();
	}

	[[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
		return matrix_.rbegin();
	}
	[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
		return matrix_.rbegin();
	}

	[[nodiscard]] constexpr reverse_iterator rend() noexcept {
		return matrix_.rend();
	}
	[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
		return matrix_.rend();
	}

	[[nodiscard]] constexpr ValueType& at(size_t index) {
		if (vector_.size() <= index) {
			throw Lamb::Error::Code<Matrix>("out of range", __func__);
		}

		return vector_.at(index);
	}

	[[nodiscard]] constexpr const ValueType& at(size_t index) const {
		if (vector_.size() <= index) {
			throw Lamb::Error::Code<Matrix>("out of range", __func__);
		}
		return vector_.at(index);
	}

	[[nodiscard]] ValueType* data() noexcept {
		return reinterpret_cast<ValueType*>(this);
	}

	[[nodiscard]] const ValueType* data() const noexcept {
		return reinterpret_cast<const ValueType*>(this);
	}

	[[nodiscard]] constexpr VectorType& view() noexcept {
		return vector_;
	}
	[[nodiscard]] constexpr const VectorType& view() const noexcept {
		return vector_;
	}

	[[noreturn]] constexpr void swap(Matrix& right) {
		vector_.swap(right.vector_);
	}


/// <summary>
/// 正方行列のみ
/// </summary>
public:
	static [[nodiscard]] const Matrix& Identity() requires (height == width)
	{
		static Matrix identity;

		for (size_t i = 0; i < height; i++) {
			identity[i][i] = 1.0f;
		}

		return identity;
	}

	[[nodiscard]] Matrix Inverse() const requires (height == width) {
		if constexpr (height == 1) {
			return *this;
		}

		Matrix tmp = *this;

		const Matrix& kIdentity = Identity();
		Matrix identity = kIdentity;

		ValueType toOne = tmp[0][0];

		ValueType tmpNum = 0.0f;

		for (int i = 0; i < height; i++) {
			if (tmp.matrix_[i][i] == 0.0f && i < height) {
				int pibIndex = i;
				float pibot = fabsf(tmp.matrix_[i][i]);

				for (int y = i + 1; y < height; y++) {
					if (tmp.matrix_[y][i] != 0.0f && pibot < fabsf(tmp.matrix_[y][i])) {
						pibot = fabsf(tmp.matrix_[y][i]);
						pibIndex = y;
					}
				}

				if (pibot == 0.0f) {
					return kIdentity;
				}

				tmp.matrix_[i].swap(tmp.matrix_[pibIndex]);
				identity.matrix_[i].swap(identity.matrix_[pibIndex]);
			}

			toOne = tmp.matrix_[i][i];
			for (int x = 0; x < height; x++) {
				tmp.matrix_[i][x] /= toOne;
				identity.matrix_[i][x] /= toOne;
			}

			for (int y = 0; y < height; ++y) {
				if (i == y) {
					continue;
				}

				tmpNum = -tmp.matrix_[y][i];
				for (int x = 0; x < width; x++) {
					tmp.matrix_[y][x] += tmpNum * tmp.matrix_[i][x];
					identity.matrix_[y][x] += tmpNum * identity.matrix_[i][x];
				}
			}
		}

		if (tmp != kIdentity) {
			return kIdentity;
		}

		return identity;
	}

	[[nodiscard]] Matrix Transepose() const requires (height == width) {
		Matrix result;

		for (size_t y = 0; y < height; y ++) {
			for (size_t x = 0; x < width; x++) {
				result[y][x] = matrix_[x][y];
			}
		}

		return result;
	}

public:
	[[nodiscard]] std::string GetString() const {
		std::string str;

		for (auto& line : *this) {
			for (auto& i : line) {
				str += std::to_string(i) + ", ";
			}
			str += "\n";
		}

		return str;
	}


protected:
	union {
		MatrixType matrix_;
		VectorType vector_;
	};
};