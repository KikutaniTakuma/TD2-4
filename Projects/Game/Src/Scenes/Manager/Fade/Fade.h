#pragma once
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include "Utils/Easing/Easing.h"

class Fade {
public:
	Fade();
	Fade(const Fade&) = default;
	Fade(Fade&&) = default;
	~Fade() = default;

	Fade& operator=(const Fade&) = default;
	Fade& operator=(Fade&&) = default;

public:
	/// <summary>
	/// フェードアウトをスタートさせる
	/// </summary>
	void OutStart();

	/// <summary>
	/// フェードインをスタートさせる
	/// </summary>
	void InStart();

	/// <summary>
	/// フェードアウトが終わった瞬間を取得
	/// </summary>
	/// <returns>終わった瞬間true</returns>
	bool OutEnd() const;

	/// <summary>
	/// フェードアウト中
	/// </summary>
	/// <returns>フェードアウト中はtrue</returns>
	bool OutStay() const;

	/// <summary>
	/// フェードインが終わった瞬間を取得
	/// </summary>
	/// <returns>終わった瞬間</returns>
	bool InEnd() const;

	/// <summary>
	/// フェードイン中
	/// </summary>
	/// <returns>フェードイン中はtrue</returns>
	bool InStay() const;

	/// <summary>
	/// フェード中か
	/// </summary>
	/// <returns></returns>
	bool IsActive() const;

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update();

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="viewProjection">カメラ行列</param>
	void Draw(const Mat4x4& viewProjection);

private:
	/// <summary>
	/// フェードインしてるか
	/// </summary>
	Lamb::Flg isInStart_;

	/// <summary>
	/// フェードアウトしてるか
	/// </summary>
	Lamb::Flg isOutStart_;

	Texture2D tex_;

	Easing ease_;
	/// <summary>
	/// seconds
	/// </summary>
	float fadeTime_;
};