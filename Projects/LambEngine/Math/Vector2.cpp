#include "Vector2.h"
#include <assert.h>
#include <cmath>
#include <numbers>

#include "Error/Error.h"

constexpr Vector2::Vector2() noexcept :
	x(0.0f),
	y(0.0f)
{}

Vector2::Vector2(float x, float y) noexcept :
	x(x),
	y(y)
{}

const Vector2 Vector2::kIdentity = { 1.0f,1.0f };
const Vector2 Vector2::kZero = {0.0f, 0.0f};
const Vector2 Vector2::kXIdentity = { 1.0f,0.0f };
const Vector2 Vector2::kYIdentity = { 0.0f,1.0f };


Vector2 Vector2::operator+(const Vector2& right) const noexcept {
	Vector2 result;

	result.x = this->x + right.x;
	result.y = this->y + right.y;

	return result;
};
Vector2 Vector2::operator-(const Vector2& right) const noexcept {
	Vector2 result;

	result.x = this->x - right.x;
	result.y = this->y - right.y;

	return result;
}


Vector2 Vector2::operator*(float scalar) const noexcept {
	Vector2 result;

	result.x = this->x * scalar;
	result.y = this->y * scalar;

	return result;
}
Vector2 Vector2::operator/(float scalar) const noexcept {
	Vector2 result;

	result.x = this->x / scalar;
	result.y = this->y / scalar;

	return result;
}

Vector2& Vector2::operator=(const Vector2& right) noexcept {
	this->x = right.x;
	this->y = right.y;

	return *this;
}

Vector2& Vector2::operator=(Vector2&& right) noexcept {
	this->x = std::move(right.x);
	this->y = std::move(right.y);

	return *this;
}


Vector2& Vector2::operator+=(const Vector2& right) noexcept {
	this->x += right.x;
	this->y += right.y;

	return *this;
}
Vector2& Vector2::operator-=(const Vector2& right) noexcept {
	this->x -= right.x;
	this->y -= right.y;

	return *this;
}

Vector2& Vector2::operator*=(float scalar) noexcept {
	this->x *= scalar;
	this->y *= scalar;

	return *this;
}
Vector2& Vector2::operator/=(float scalar) noexcept {
	this->x /= scalar;
	this->y /= scalar;

	return *this;
}

bool Vector2::operator==(const Vector2& right) const noexcept {
	return this->x == right.x && this->y == right.y;
}
bool Vector2::operator!=(const Vector2& right) const noexcept {
	return this->x != right.x || this->y != right.y;
}

float& Vector2::operator[](size_t index) {
	if (size() <= index) {
		throw Lamb::Error::Code<Vector2>("index is over", ErrorPlace);
	}
	return data()[index];
}
const float& Vector2::operator[](size_t index) const {
	if (size() <= index) {
		throw Lamb::Error::Code<Vector2>("index is over", ErrorPlace);
	}
	return data()[index];
}

void Vector2::Rotate(float rad) noexcept {
	x *= cosf(rad);
	y *= sinf(rad);
}

float Vector2::Cross(const Vector2& right) const noexcept {
	return x * right.y - y * right.x;
}
float Vector2::Dot(const Vector2& right) const noexcept {
	return x * right.x + y * right.y;
}

float Vector2::Length() const noexcept {
	return std::sqrt(x * x + y* y);
}
float Vector2::LengthSQ() const noexcept {
	return x * x + y * y;
}

Vector2 Vector2::Normalize() const noexcept {
	if (*this == Vector2::kZero) {
		return *this;
	}
	return *this / Length();
}

float Vector2::GetRad() const noexcept {
	if (*this == Vector2::kZero) {
		return 0.0f;
	}

	return std::atan2(y, x);
}

float& Vector2::at(size_t index) {
	if (size() <= index) {
		throw Lamb::Error::Code<Vector2>("index is over", ErrorPlace);
	}
	return data()[index];
}

const float& Vector2::at(size_t index) const {
	if (size() <= index) {
		throw Lamb::Error::Code<Vector2>("index is over", ErrorPlace);
	}
	return data()[index];
}

Vector2 Vector2::Lerp(const Vector2& start, const Vector2& end, float easeSpd) {
	Vector2 result;

	result.x = std::lerp(start.x, end.x, easeSpd);
	result.y = std::lerp(start.y, end.y, easeSpd);

	return result;
}