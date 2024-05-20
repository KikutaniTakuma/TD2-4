#pragma once
#include "Utils/SafePtr/SafePtr.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include "Utils/Easeing/Easeing.h"
#include "Math/Transform.h"
#include <memory>

class SimpleFade{
public:
	SimpleFade();
	SimpleFade(const SimpleFade&) = default;
	SimpleFade(SimpleFade&&) = default;
	~SimpleFade() = default;

	SimpleFade& operator=(const SimpleFade&) = default;
	SimpleFade& operator=(SimpleFade&&) = default;

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

	void SetFadeTime(const float fadeTime) {
		fadeTime_ = fadeTime;
	}

	void SetInOutColor(const uint32_t inColor, const uint32_t outColor) {
		fadeInColor_ = inColor;
		fadeOutColor_ = outColor;
	}

	void SetTransZ(const float posZ) {
		transform_->translate.z = posZ;
	}

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

	Lamb::SafePtr<Texture2D> tex_;
	std::unique_ptr<Transform> transform_;
	uint32_t color_;

	uint32_t fadeInColor_;
	uint32_t fadeOutColor_;

	Easeing ease_;
	/// <summary>
	/// seconds
	/// </summary>
	float fadeTime_;

};

