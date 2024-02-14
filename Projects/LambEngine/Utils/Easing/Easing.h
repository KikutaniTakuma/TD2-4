#pragma once
#include <functional>
#include <string>
#include <algorithm>
#include <cassert>
#include <concepts>
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include"Utils/Flg/Flg.h"

/// <summary>
/// イージング管理
/// </summary>
class Easing {
public:
	Easing();
	Easing(const Easing& right) = default;
	Easing(Easing&& right) noexcept = default;
	~Easing() = default;

public:
	Easing& operator=(const Easing& right) = default;
	Easing& operator=(Easing&& right) noexcept = default;

public:
	void Update();

	/// <summary>
	/// イージングスタート
	/// </summary>
	/// <param name="start">スタート</param>
	/// <param name="end">エンド</param>
	/// <param name="isLoop">ループするか</param>
	/// <param name="easeTime">何秒でイージングするか</param>
	/// <param name="ease">イージング関数</param>
	void Start(
		bool isLoop,
		float easeTime,
		std::function<float(float)> ease = [](float t) {
			return t;
		}
	);
	
	/// <summary>
	/// リスタート
	/// </summary>
	void Restart();

	/// <summary>
	/// 再スタート
	/// </summary>
	void Pause();

	/// <summary>
	/// 強制終了
	/// </summary>
	void Stop();

	/// <summary>
	/// スタートとエンドを入れるとtの値に応じた線形補完された値が返る
	/// </summary>
	/// <typeparam name="T">
	/// 数値型とVector2とVector3のみ可能(ユーザー定義型とポインタ型は不可)
	/// </typeparam>
	/// <param name="start">始め</param>
	/// <param name="end">終わり</param>
	/// <returns>線形補完された値</returns>
	template<typename T>
	T Get(const T& start, const T& end) {
		static_assert(!std::is_pointer<T>::value, "Do not use pointer types");
		return static_cast<T>(std::lerp<T>(start, end, ease_(t_)));
	}

	Vector2 Get(const Vector2& start, const Vector2& end) {
		return Vector2::Lerp(start, end, ease_(t_));
	}

	Vector3 Get(const Vector3& start, const Vector3& end) {
		return Vector3::Lerp(start, end, ease_(t_));
	}

	Vector4 Get(const Vector4& start, const Vector4& end) {
		return ColorLerp(start, end, ease_(t_));
	}

	float GetT() const {
		return ease_(t_);
	}

	void Debug(const std::string& debugName);
	void DebugTreeNode(const std::string& debugName);

	bool ActiveEnter() const {
		return isActive_.OnEnter();
	}

	bool ActiveStay() const {
		return isActive_.OnStay();
	}

	bool ActiveExit() const {
		return isActive_.OnExit();
	}

	const Lamb::Flg& GetIsActive() const {
		return isActive_;
	}

private:
#ifdef _DEBUG
	int32_t easeType_;
	float easeTime_;
#endif // _DEBUG

	std::function<float(float)> ease_;

	Lamb::Flg isActive_;
	Lamb::Flg isLoop_;

	float t_;

	float spdT_;

public:
	template<std::integral IsInt>
	static std::function<float(float)> GetFunction(IsInt typeNum) {
		std::function<float(float)> ease;

		switch (typeNum)
		{
		default:
		case 0:
			ease = [](float t) {
				return t;
				};
			break;


		case 1:
			ease = InSine;
			break;
		case 2:
			ease = OutSine;
			break;
		case 3:
			ease = InOutSine;
			break;


		case 4:
			ease = InQuad;
			break;
		case 5:
			ease = OutQuad;
			break;
		case 6:
			ease = InOutQuad;
			break;


		case 7:
			ease = InCubic;
			break;
		case 8:
			ease = OutCubic;
			break;
		case 9:
			ease = InOutCubic;
			break;


		case 10:
			ease = InQuart;
			break;
		case 11:
			ease = OutQuart;
			break;
		case 12:
			ease = InOutQuart;
			break;


		case 13:
			ease = InQuint;
			break;
		case 14:
			ease = OutQuint;
			break;
		case 15:
			ease = InOutQuint;
			break;


		case 16:
			ease = InExpo;
			break;
		case 17:
			ease = OutExpo;
			break;
		case 18:
			ease = InOutExpo;
			break;


		case 19:
			ease = InCirc;
			break;
		case 20:
			ease = OutCirc;
			break;
		case 21:
			ease = InOutCirc;
			break;


		case 22:
			ease = InBack;
			break;
		case 23:
			ease = OutBack;
			break;
		case 24:
			ease = InOutBack;
			break;


		case 25:
			ease = InElastic;
			break;
		case 26:
			ease = OutElastic;
			break;
		case 27:
			ease = InOutElastic;
			break;


		case 28:
			ease = InBounce;
			break;
		case 29:
			ease = OutBounce;
			break;
		case 30:
			ease = InOutBounce;
			break;
		}

		return ease;
	}

/// <summary>
///	Easing関数 参照 : https://easings.net/ja
/// </summary>
public:
	static float InSine(float t);
	static float OutSine(float t);
	static float InOutSine(float t);

	static float InQuad(float t);
	static float OutQuad(float t);
	static float InOutQuad(float t);

	static float InCubic(float t);
	static float OutCubic(float t);
	static float InOutCubic(float t);
	
	static float InQuart(float t);
	static float OutQuart(float t);
	static float InOutQuart(float t);

	static float InQuint(float t);
	static float OutQuint(float t);
	static float InOutQuint(float t);

	static float InExpo(float t);
	static float OutExpo(float t);
	static float InOutExpo(float t);

	static float InCirc(float t);
	static float OutCirc(float t);
	static float InOutCirc(float t);

	static float InBack(float t);
	static float OutBack(float t);
	static float InOutBack(float t);

	static float InElastic(float t);
	static float OutElastic(float t);
	static float InOutElastic(float t);

	static float InBounce(float t);
	static float OutBounce(float t);
	static float InOutBounce(float t);
};