#pragma once
#include <string>
#include "Math/Mat4x4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

class Tex2DAniamtor {
public:
	Tex2DAniamtor() = default;
	Tex2DAniamtor(const Tex2DAniamtor&) = default;
	Tex2DAniamtor(Tex2DAniamtor&&) = default;
	~Tex2DAniamtor() = default;

	Tex2DAniamtor& operator=(const Tex2DAniamtor&) = default;
	Tex2DAniamtor& operator=(Tex2DAniamtor&&) = default;

public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

public:
	// 最初からスタート
	void Start();

	// 止めてリセット
	void Stop();

	// リセット
	void Reset();

	// スタート
	void Restart();

	// 一時停止
	void Pause();

public:
	/// <summary>
	/// アニメーションのマトリックスを取得
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] const Mat4x4& GetUvMat4x4() const {
		return animationMatrix_;
	}

	/// <summary>
	/// アニメーションの枚数
	/// </summary>
	/// <param name="animationNumber"></param>
	void SetAnimationNumber(uint32_t animationNumber);

	/// <summary>
	/// アニメーションの開始位置
	/// </summary>
	void SetStartPos(Vector2 startPos) {
		startPos_ = startPos;
	}

	/// <summary>
	/// <para>アニメーションをループさせるか</para>
	/// <para>isFullAnimationがfalseの場合、設定されたindexのアニメーションでループする</para>
	/// </summary>
	/// <param name="isLoop"></param>
	void SetLoopAnimation(bool isLoop) {
		isLoop_ = isLoop;
	}

	/// <summary>
	/// 次の画像に行くまでの時間
	/// </summary>
	/// <param name="duration">秒数</param>
	void SetDuration(float duration) {
		duration_ = duration;
	}

	/// <summary>
	/// 上下反転
	/// </summary>
	/// <param name="isSingned">trueなら反転</param>
	void FlipUpsideDown(bool isSingned);

	/// <summary>
	/// 左右反転
	/// </summary>
	/// <param name="isSingned">trueなら反転</param>
	void FlipHorizontal(bool isSingned);

private:
	Vector3 startPos_;
	Vector3 currentPos_;
	Vector3 scale_ = Vector3::kIdentity;
	uint32_t currentAnimationNumber_ = 0u;
	uint32_t animationNumber_ = 0u;

	float animationTime_ = 0.0f;
	float duration_ = 0.0f;
	Mat4x4 animationMatrix_;

	bool isActive_ = false;
	bool isLoop_ = false;
};