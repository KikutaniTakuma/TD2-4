#pragma once
#include <array>
#include <type_traits>
#include <string>

#include "Error/Error.h"

template<std::floating_point floatingType, size_t height, size_t width>
class Matrix {
public:
	using value_type = floatingType;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;


	using width_type = std::array<value_type, width>;
	using width_pointer = width_type*;
	using width_const_pointer = const width_type*;
	using width_reference = width_type&;
	using width_const_reference = const width_type&;
	using height_type = std::array<width_type, height>;

	using vector_type = std::array<value_type, height* width>;
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


#define value_cast(num) static_cast<value_type>(num)

public:
	constexpr Matrix() noexcept :
		vector_()
	{}

	constexpr Matrix(const vector_type& right) noexcept :
		vector_(right)
	{}
	template<std::floating_point othertype, size_t otherHeight, size_t otherWidth>
	constexpr Matrix(const Matrix<othertype, otherHeight, otherWidth>& right) noexcept {
		*this = right;
	}
	template<std::floating_point othertype, size_t otherHeight, size_t otherWidth>
	constexpr Matrix(const std::array<std::array<othertype, otherWidth>, otherHeight>& right) noexcept {
		*this = right;
	}
	constexpr Matrix(const Matrix&) = default;
	constexpr Matrix(Matrix&&) = default;

private:
	constexpr Matrix(const value_type identity) noexcept requires (height == width)
		: Matrix()
	{
		for (size_t i = 0; i < height; i++) {
			matrix_[i][i] = identity;
		}
	}

public:
	~Matrix() = default;

public:
	constexpr Matrix& operator=(const Matrix&) = default;
	constexpr Matrix& operator=(Matrix&&) = default;

	template<std::floating_point othertype, size_t otherHeight, size_t otherWidth>
	constexpr Matrix& operator=(const Matrix<othertype, otherHeight, otherWidth>& right) {
		for (size_t y = 0; y < height and y < otherHeight; y++) {
			for (size_t x = 0; x < width and x < otherWidth; x++) {
				this->matrix_[y][x] = value_cast(right[y][x]);
			}
		}

		return *this;
	}
	template<std::floating_point othertype, size_t otherHeight, size_t otherWidth>
	constexpr Matrix& operator=(const std::array<std::array<othertype, otherWidth>, otherHeight>& right) {
		for (size_t y = 0; y < height and y < otherHeight; y++) {
			for (size_t x = 0; x < width and x < otherWidth; x++) {
				this->matrix_[y][x] = value_cast(right[y][x]);
			}
		}

		return *this;
	}

public:
	[[nodiscard]] constexpr width_reference operator[](size_t index) {
		if (height <= index) {
			OutOfRange(__func__);
		}

		return matrix_[index];
	}

	[[nodiscard]] constexpr width_const_reference operator[](size_t index) const {
		if (height <= index) {
			OutOfRange(__func__);
		}
		return matrix_[index];
	}

public:
	template<std::floating_point othertype, size_t otherHeight, size_t otherWidth>
	[[nodiscard]] constexpr Matrix<value_type, height, otherWidth> operator*(const Matrix<othertype, otherHeight, otherWidth>& right) const noexcept requires(width == otherHeight) {
		Matrix<value_type, height, otherWidth> result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < otherWidth; x++) {
				for (size_t i = 0; i < width; i++) {
					result[y][x] += matrix_[y][i] * value_cast(right[i][x]);
				}
			}
		}

		return result;
	}
	template<std::floating_point othertype, size_t otherHeight, size_t otherWidth>
	constexpr Matrix& operator*=(const Matrix<othertype, otherHeight, otherWidth>& right) noexcept requires(otherHeight == otherWidth and width == otherHeight) {
		*this = *this * right;

		return *this;
	}

	[[nodiscard]] constexpr Matrix operator+(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				result[y][x] = matrix_[y][x] + right[y][x];
			}
		}

		return result;
	}
	constexpr Matrix& operator+=(const Matrix& right)noexcept {
		*this = *this + right;

		return *this;
	}
	[[nodiscard]] constexpr Matrix operator-(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				result[y][x] = matrix_[y][x] - right[y][x];
			}
		}

		return result;
	}
	constexpr Matrix& operator-=(const Matrix& right)noexcept {
		*this = *this - right;

		return *this;
	}

public:
	[[nodiscard]] constexpr bool operator==(const Matrix& right) const {
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
		if (height <= index) {
			OutOfRange(__func__);
		}

		return matrix_.at(index);
	}

	[[nodiscard]] constexpr width_const_reference at(size_t index) const {
		if (height <= index) {
			OutOfRange(__func__);
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

	[[noreturn]] constexpr void swap(Matrix& right) {
		vector_.swap(right.vector_);
	}



	/// <summary>
	/// 正方行列のみ
	/// </summary>

public:
	static [[nodiscard]] constexpr const Matrix& Identity() requires (height == width) {
		static const Matrix identity(1.0f);

		return identity;
	}

public:
	[[nodiscard]] Matrix Inverse() const requires (height == width) {
		if constexpr (height == 1) {
			return *this;
		}

		Matrix tmp = *this;

		const Matrix& kIdentity = Identity();
		Matrix identity = kIdentity;

		value_type toOne = tmp[0][0];

		value_type tmpNum = value_cast(0.0);

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

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				result[y][x] = matrix_[x][y];
			}
		}

		return result;
	}

	/// <summary>
	/// メンバ関数
	/// </summary>
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

private:
	void OutOfRange(const std::string& funcName) const {
		throw Lamb::Error::Code<Matrix>("out of range", funcName);
	}

	/// <summary>
	/// メンバ変数
	/// </summary>
protected:
	union {
		matrix_type matrix_;
		vector_type vector_;
	};
};