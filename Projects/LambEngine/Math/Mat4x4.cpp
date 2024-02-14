#include "Mat4x4.h"
#include "Vector3.h"
#include <cmath>
#include <cassert>
#include <algorithm>
#include <format>
#include <Windows.h>
#undef max
#undef min
#include <immintrin.h>

const Mat4x4 Mat4x4::kIdentity = Mat4x4{
	std::array<Vector4, 4>{
		Vector4{1.0f, 0.0f, 0.0f, 0.0f},
		Vector4{0.0f, 1.0f, 0.0f, 0.0f},
		Vector4{0.0f, 0.0f, 1.0f, 0.0f},
		Vector4{0.0f, 0.0f, 0.0f, 1.0f}
	}
};

const Mat4x4 Mat4x4::kZero = Mat4x4{
	std::array<Vector4, 4>{
		Vector4{0.0f, 0.0f, 0.0f, 0.0f},
		Vector4{0.0f, 0.0f, 0.0f, 0.0f},
		Vector4{0.0f, 0.0f, 0.0f, 0.0f},
		Vector4{0.0f, 0.0f, 0.0f, 0.0f}
	}
};

Mat4x4::Mat4x4()
	:m_()
{}

Mat4x4::Mat4x4(const Mat4x4& mat) {
	*this = mat;
}

Mat4x4::Mat4x4(Mat4x4&& mat) noexcept {
	*this = std::move(mat);
}

Mat4x4::Mat4x4(const std::initializer_list<float>& num) {
	auto i = num.begin();
	assert(num.size() <= 16llu);
	for (size_t y = 0llu; y < 4llu; y++) {
		for (size_t x = 0; x < 4llu; x++) {
			m_[y][x] = *i;
			i++;
			if (i == num.end()) {
				return;
			}
		}
	}
}

Mat4x4::Mat4x4(const std::array<Vector4, 4>& num) {
	m_ = num;
}

Mat4x4::Mat4x4(const std::array<float, 16>& num) {
	size_t i = 0;
	for (size_t y = 0llu; y < 4llu; y++) {
		for (size_t x = 0; x < 4llu; x++) {
			m_[y][x] = num[i];
			i++;
		}
	}
}

Mat4x4& Mat4x4::operator=(const Mat4x4& mat) {
	std::copy(mat.m_.begin(), mat.m_.end(), m_.begin());

	return *this;
}

Mat4x4& Mat4x4::operator=(Mat4x4&& mat) noexcept {
	m_ = std::move(mat.m_);

	return *this;
}

Mat4x4 Mat4x4::operator*(const Mat4x4& mat) const {
	Mat4x4 result;

	for (int y = 0; y < Mat4x4::kHeight_; y++) {
		for (int x = 0; x < Mat4x4::kWidth_; x++) {
			for (int i = 0; i < Mat4x4::kWidth_; i++) {
				result.m_[y][x] += this->m_[y][i] * mat.m_[i][x];
			}
		}
	}

	return result;
}

Mat4x4& Mat4x4::operator*=(const Mat4x4& mat) {
	*this = *this * mat;

	return *this;
}

Mat4x4 Mat4x4::operator+(const Mat4x4& mat) const {
	Mat4x4 tmp;

	for (int y = 0; y < Mat4x4::kHeight_; y++) {
		for (int x = 0; x < Mat4x4::kWidth_; x++) {
			tmp[y][x] = this->m_[y][x] + mat.m_[y][x];
		}
	}

	return tmp;
}
Mat4x4& Mat4x4::operator+=(const Mat4x4& mat) {
	for (int y = 0; y < Mat4x4::kHeight_; y++) {
		for (int x = 0; x < Mat4x4::kWidth_; x++) {
			this->m_[y][x] += mat.m_[y][x];
		}
	}

	return *this;
}
Mat4x4 Mat4x4::operator-(const Mat4x4& mat) const {
	Mat4x4 tmp;

	for (int y = 0; y < Mat4x4::kHeight_; y++) {
		for (int x = 0; x < Mat4x4::kWidth_; x++) {
			tmp[y][x] = this->m_[y][x] - mat.m_[y][x];
		}
	}

	return tmp;
}
Mat4x4& Mat4x4::operator-=(const Mat4x4& mat) {
	for (int y = 0; y < Mat4x4::kHeight_; y++) {
		for (int x = 0; x < Mat4x4::kWidth_; x++) {
			this->m_[y][x] -= mat.m_[y][x];
		}
	}

	return *this;
}

bool Mat4x4::operator==(const Mat4x4& mat) const {
	return m_ == mat.m_;
}

bool Mat4x4::operator!=(const Mat4x4& mat) const {
	return m_ != mat.m_;
}

const Mat4x4& Mat4x4::Identity() {
	*this = Mat4x4::kIdentity;

	return *this;
}

const Mat4x4& Mat4x4::Translate(const Vector3& vec) {
	this->m_ = {};

	this->m_[0][0] = 1.0f;
	this->m_[1][1] = 1.0f;
	this->m_[2][2] = 1.0f;
	this->m_[3][3] = 1.0f;

	this->m_[3][0] = vec.x;
	this->m_[3][1] = vec.y;
	this->m_[3][2] = vec.z;

	return *this;
}

const Mat4x4& Mat4x4::Scalar(const Vector3& vec) {
	this->m_ = {};

	this->m_[0][0] = vec.x;
	this->m_[1][1] = vec.y;
	this->m_[2][2] = vec.z;
	this->m_[3][3] = 1.0f;

	return *this;
}

const Mat4x4& Mat4x4::RotateX(float rad) {
	this->m_ = {};
	this->m_[0][0] = 1.0f;
	this->m_[3][3] = 1.0f;

	this->m_[1][1] = std::cos(rad);
	this->m_[1][2] = std::sin(rad);
	this->m_[2][1] = -std::sin(rad);
	this->m_[2][2] = std::cos(rad);

	return *this;
}

const Mat4x4& Mat4x4::RotateY(float rad) {
	this->m_ = {};
	this->m_[1][1] = 1.0f;
	this->m_[3][3] = 1.0f;

	this->m_[0][0] = std::cos(rad);
	this->m_[0][2] = -std::sin(rad);
	this->m_[2][0] = std::sin(rad);
	this->m_[2][2] = std::cos(rad);

	return *this;
}

const Mat4x4& Mat4x4::RotateZ(float rad) {
	this->m_ = {};
	this->m_[2][2] = 1.0f;
	this->m_[3][3] = 1.0f;
	
	this->m_[0][0] = std::cos(rad);
	this->m_[0][1] = -std::sin(rad);
	this->m_[1][0] = std::sin(rad);
	this->m_[1][1] = std::cos(rad);

	return *this;
}

const Mat4x4& Mat4x4::Affin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 rotate = Mat4x4::MakeRotateX(rad.x) * Mat4x4::MakeRotateY(rad.y) * Mat4x4::MakeRotateZ(rad.z);

	*this = Mat4x4{ 
		scale.x * rotate.m_[0][0], scale.x * rotate.m_[0][1],scale.x * rotate.m_[0][2], 0.0f,
		scale.y * rotate.m_[1][0], scale.y * rotate.m_[1][1],scale.y * rotate.m_[1][2], 0.0f,
		scale.z * rotate.m_[2][0], scale.z * rotate.m_[2][1],scale.z * rotate.m_[2][2], 0.0f,
		translate.x, translate.y, translate.z, 1.0f
	};

	return *this;
}


const Mat4x4& Mat4x4::Inverse() {
	Mat4x4 tmp = *this;

	Mat4x4 identity = Mat4x4::kIdentity;

	float toOne = tmp[0][0];

	float tmpNum = 0.0f;

	for (int i = 0; i < Mat4x4::kHeight_; i++) {
		if (tmp.m_[i][i] == 0.0f && i < Mat4x4::kHeight_) {
			int pibIndex = i;
			float pibot = fabsf(tmp.m_[i][i]);

			for (int y = i + 1; y < Mat4x4::kHeight_; y++) {
				if (tmp.m_[y][i] != 0.0f && pibot < fabsf(tmp.m_[y][i])) {
					pibot = fabsf(tmp.m_[y][i]);
					pibIndex = y;
				}
			}

			if (pibot == 0.0f) {
				return *this;
			}

			tmp.m_[i].m.swap(tmp.m_[pibIndex].m);
			identity.m_[i].m.swap(identity.m_[pibIndex].m);
		}

		toOne = tmp.m_[i][i];
		for (int x = 0; x < Mat4x4::kHeight_; x++) {
			tmp.m_[i][x] /= toOne;
			identity.m_[i][x] /= toOne;
		}

		for (int y = 0; y < Mat4x4::kHeight_; ++y) {
			if (i == y) {
				continue;
			}

			tmpNum = -tmp.m_[y][i];
			for (int x = 0; x < Mat4x4::kWidth_; x++) {
				tmp.m_[y][x] += tmpNum * tmp.m_[i][x];
				identity.m_[y][x] += tmpNum * identity.m_[i][x];
			}
		}
	}

	if (tmp != kIdentity) {
		return *this;
	}

	*this = identity;

	return *this;
}


const Mat4x4& Mat4x4::Transepose() {
	std::swap(m_[1][0], m_[0][1]);
	std::swap(m_[2][0], m_[0][2]);
	std::swap(m_[3][0], m_[0][3]);
	std::swap(m_[2][1], m_[1][2]);
	std::swap(m_[2][3], m_[3][2]);
	std::swap(m_[3][1], m_[1][3]);

	return *this;
}

const Mat4x4& Mat4x4::PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	m_ = {};

	m_[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
	m_[1][1] = 1.0f / std::tan(fovY / 2.0f);
	m_[2][2] = farClip / (farClip - nearClip);
	m_[2][3] = 1.0f;
	m_[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return *this;
}

const Mat4x4& Mat4x4::Orthographic(float left, float top, float right, float bottom, float nearClip, float farClip) {
	m_ = {};

	m_[0][0] = 2.0f / (right - left);
	m_[1][1] = 2.0f / (top - bottom);
	m_[2][2] = 1.0f / (farClip - nearClip);
	m_[3][3] = 1.0f;

	m_[3][0] = (left + right) / (left - right);
	m_[3][1] = (top + bottom) / (bottom - top);
	m_[3][2] = nearClip / (nearClip - farClip);

	return *this;
}

const Mat4x4& Mat4x4::ViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	m_ = {};

	m_[0][0] = width / 2.0f;
	m_[1][1] = height/ -2.0f;
	m_[2][2] = maxDepth - minDepth;
	m_[3][3] = 1.0f;

	m_[3][0] = left + (width / 2.0f);
	m_[3][1] = top + (height / 2.0f);
	m_[3][2] = minDepth;

	return *this;
}

Mat4x4 Mat4x4::MakeInverse(Mat4x4 mat) {
	Mat4x4 result = mat;
	result.Inverse();
	return result;
}

Mat4x4 Mat4x4::MakeTransepose(Mat4x4 mat) {
	Mat4x4 result = mat;
	result.Transepose();
	return result;
}

Mat4x4 Mat4x4::MakeTranslate(Vector3 vec) {
	Mat4x4 mat;

	mat.Translate(vec);

	return mat;
}

Mat4x4 Mat4x4::MakeScalar(Vector3 vec) {
	Mat4x4 mat;

	mat.Scalar(vec);

	return mat;
}

Mat4x4 Mat4x4::MakeRotateX(float rad) {
	Mat4x4 result;

	result.RotateX(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotateY(float rad) {
	Mat4x4 result;

	result.RotateY(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotateZ(float rad) {
	Mat4x4 result;

	result.RotateZ(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotate(const Vector3& rad) {
	Mat4x4 result;

	result.Affin(Vector3::kIdentity, rad, Vector3::kZero);

	return result;
}

Mat4x4 Mat4x4::MakeAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 result;

	result.Affin(scale, rad, translate);

	return result;
}

Mat4x4 Mat4x4::MakeAffin(const Vector3& scale, const Vector3& from, const Vector3& to, const Vector3& translate) {
	Mat4x4 result;

	result = Mat4x4::MakeScalar(scale) * Mat4x4::DirectionToDirection(from, to) * Mat4x4::MakeTranslate(translate);

	return result;
}

Mat4x4 Mat4x4::MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	Mat4x4 result;

	result.PerspectiveFov(fovY, aspectRatio, nearClip, farClip);

	return result;
}

Mat4x4 Mat4x4::MakeOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip) {
	Mat4x4 result;

	result.Orthographic(left, right, top, bottom, nearClip, farClip);

	return result;
}

Mat4x4 Mat4x4::MakeViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Mat4x4 result;

	result.ViewPort(left, top, width, height, minDepth, maxDepth);

	return result;
}

Mat4x4 Mat4x4::DirectionToDirection(const Vector3& from, const Vector3& to) {
	Vector3 normal;

	if (from.Dot(to) == -1.0f) {
		if (from.x != 0.0f || from.y != 0.0f) {
			normal = Vector3{ from.y, -from.x, 0.0f }.Normalize();
		}
		else if (from.x != 0.0f || from.z != 0.0f) {
			normal = Vector3{ from.z, 0.0f, -from.x }.Normalize();
		}
	}
	else {
		normal = from.Cross(to).Normalize();
	}


	float theataCos = from.Dot(to);
	float theataSin = from.Cross(to).Length();

	Mat4x4 result = Mat4x4{
		std::array<Vector4, 4>{
			Vector4{ 
				normal.x * normal.x * (1.0f - theataCos) + theataCos,
				normal.x * normal.y * (1.0f - theataCos) + normal.z * theataSin,
				normal.x * normal.z * (1.0f - theataCos) - normal.y * theataSin,
				0.0f
			},
			Vector4{
				normal.y* normal.x* (1.0f - theataCos) - normal.z * theataSin,
				normal.y* normal.y* (1.0f - theataCos) + theataCos,
				normal.y* normal.z* (1.0f - theataCos) + normal.x * theataSin,
				0.0f
			},
			Vector4{
				normal.z * normal.x * (1.0f - theataCos) + normal.y * theataSin,
				normal.z * normal.y * (1.0f - theataCos) - normal.x * theataSin,
				normal.z * normal.z * (1.0f - theataCos) + theataCos,
				0.0f
			},
			Vector4::kWIdentity
		}
	};


	return result;
}

Mat4x4 Mat4x4::MakeRotateAxisAngle(const Vector3& axis, float angle) {
	float angleCos = std::cos(angle);
	float angleSin = std::sin(angle);

	Mat4x4 result = Mat4x4{
		std::array<Vector4, 4>{
			Vector4{
				axis.x * axis.x * (1.0f - angleCos) + angleCos,
				axis.x * axis.y * (1.0f - angleCos) + axis.z * angleSin,
				axis.x * axis.z * (1.0f - angleCos) - axis.y * angleSin,
				0.0f
			},
			Vector4{
				axis.y * axis.x * (1.0f - angleCos) - axis.z * angleSin,
				axis.y * axis.y * (1.0f - angleCos) + angleCos,
				axis.y * axis.z * (1.0f - angleCos) + axis.x * angleSin,
				0.0f
			},
			Vector4{
				axis.z * axis.x * (1.0f - angleCos) + axis.y * angleSin,
				axis.z * axis.y * (1.0f - angleCos) - axis.x * angleSin,
				axis.z * axis.z * (1.0f - angleCos) + angleCos,
				0.0f
			},
			Vector4::kWIdentity
		}
	};


	return result;
}

std::string Mat4x4::GetMatrixString() {
	std::string result =
	std::format(
		"{:6.3f}, {:6.3f}, {:6.3f}, {:6.3f}\n{:6.3f}, {:6.3f}, {:6.3f}, {:6.3f}\n{:6.3f}, {:6.3f}, {:6.3f}, {:6.3f}\n{:6.3f}, {:6.3f}, {:6.3f}, {:6.3f}\n",
		(*this)[0][0], (*this)[0][1], (*this)[0][2], (*this)[0][3],
		(*this)[1][0], (*this)[1][1], (*this)[1][2], (*this)[1][3],
		(*this)[2][0], (*this)[2][1], (*this)[2][2], (*this)[2][3],
		(*this)[3][0], (*this)[3][1], (*this)[3][2], (*this)[3][3]
	);

	return result;
}