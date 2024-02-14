#pragma once

#include <random>
#include <algorithm>
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Lamb {
	extern std::random_device seed;
	extern std::mt19937_64 rnd;

	/// <summary>
	/// 整数型のランダム関数
	/// </summary>
	/// <typeparam name="T">32bit以下の整数型のみサポート</typeparam>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>ランダムな値</returns>
	template<IsInt T> requires requires { sizeof(T) <= sizeof(long); }
	T Random(T min, T max) {
		if (max < min) {
			std::swap(min, max);
		}

		std::uniform_int_distribution<T> dist{ min, max };

		return static_cast<T>(dist(rnd));
	}

	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	float Random(float min, float max);
	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	double Random(double min, double max);

	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	Vector2 Random(const Vector2& min, const Vector2& max);
	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	Vector3 Random(const Vector3& min, const Vector3& max);

	/// <summary>
	/// 色のランダム
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	uint32_t RandomColor(uint32_t min, uint32_t max);
	/// <summary>
	/// 色のランダム
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	Vector4 RandomColor(const Vector4& min, const Vector4& max);
}