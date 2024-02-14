#pragma once
#include <string>
#include "Math/Vector2.h"
#include "Math/Quaternion.h"
#include <concepts>

/// <summary>
/// 文字列描画(座標系はスクリーンなので注意)
/// </summary>
class StringOut {
public:
	StringOut();
	StringOut(const StringOut&) = default;
	StringOut(StringOut&&) noexcept = default;
	StringOut(const std::string& formatName);
	StringOut(const std::wstring& formatName);
	~StringOut() = default;

public:
	StringOut& operator=(const StringOut&)  =default;
	StringOut& operator=(StringOut&&) noexcept = default;
	StringOut& operator=(const std::string& right);
	StringOut& operator=(const std::wstring& right);

	StringOut& operator+=(const std::string& right);
	StringOut& operator+=(const std::wstring& right);

	StringOut& operator<<(const std::string& right);
	StringOut& operator<<(const std::wstring& right);

	template<std::integral T>
	StringOut& operator<<(const T& right) {
		str_ += std::to_wstring(right);

		return *this;
	}

	template<std::floating_point T>
	StringOut& operator<<(const T& right) {
		str_ += std::to_wstring(right);

		return *this;
	}

	StringOut& operator<<(const Vector2& right);
	StringOut& operator<<(const Vector3& right);
	StringOut& operator<<(const Vector4& right);
	StringOut& operator<<(const Quaternion& right);

	StringOut& operator>>(std::string& right);
	StringOut& operator>>(std::wstring& right);

public:
	void Draw();

	void Clear();

	void Debug(const std::string& debugName);


private:
	std::string format_;
	std::wstring str_;

#ifdef _DEBUG
	std::string debugStr_;
#endif // _DEBUG


public:
	Vector2 pos;
	float rotation;
	Vector2 scale;
	uint32_t color;
	bool isHorizontal;

public:
	void SetFormat(const std::string& formatName);
	void SetFormat(const std::wstring& formatName);
};