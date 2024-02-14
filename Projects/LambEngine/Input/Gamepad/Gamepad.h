#pragma once

#include <Windows.h>
#undef max
#undef min
#define DIRECTINPUT_VERSION 0x0800
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")
#include <cstdint>

// Xボックスコントローラーでやったのでコメントアウトの説明はそれ
// ほかのコントローラーは分からないので検証必須
// steamなどでpadの設定を行っている場合その設定のほうが優先されてしまうのでアプリを落として使用してください

/// <summary>
/// ゲームパッドの入力
/// </summary>
class Gamepad {
public:
	// 押し込みのボタン
	enum class Button {
		UP = 0, // 十字キー上
		DOWN, // 十字キー下
		LEFT, // 十字キー左
		RIGHT, // 十字キー右
		START, // 漢字の三みたいなやつ
		BACK, // 四角が重なったようなやつ
		LEFT_THUMB, // 左スティック押し込み
		RIGHT_THUMB, // 右スティック押し込み 
		LEFT_SHOULDER, // LB
		RIGHT_SHOULDER, // RB
		A = 12, // A
		B, // B
		X, // X
		Y, // Y
	};

	// RT,LT
	enum class Triger {
		LEFT,
		RIGHT,
	};

	// スティック
	enum class Stick {
		LEFT_X,
		LEFT_Y,
		RIGHT_X,
		RIGHT_Y
	};

private:
	Gamepad();
	Gamepad(const Gamepad&) = delete;
	Gamepad(Gamepad&&) = delete;
	~Gamepad() = default;
	Gamepad& operator=(const Gamepad&) = delete;
	Gamepad& operator=(Gamepad&&) = delete;

private:
	WORD preButton_;
	XINPUT_STATE state_;
	XINPUT_VIBRATION vibration_;

public:
	static Gamepad*const GetInstance();


public:
	/// <summary>
	/// 入力処理
	/// </summary>
	void Input();

	void InputReset();

public:
	/// <summary>
	/// ボタンの状態取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	bool GetButton(Button type);

	/// <summary>
	/// 前フレームのボタンの状態取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	bool GetPreButton(Button type);

	/// <summary>
	/// 押した瞬間かを取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	bool Pushed(Button type);

	/// <summary>
	/// 押し続けているかを取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	bool LongPushed(Button type);

	/// <summary>
	/// ボタンを離した瞬間かを取得
	/// </summary>
	/// <param name="type">ボタンタイプ</param>
	/// <returns>1:押された 0:押されてない</returns>
	bool Released(Button type);

	/// <summary>
	/// 何かしらのキーやトリガーが押された
	/// </summary>
	/// <returns>押されたらtrue</returns>
	bool PushAnyKey();

	/// <summary>
	/// トリガーを取得
	/// </summary>
	/// <param name="type">トリガーのタイプ</param>
	/// <returns>0.0f ～ 1.0f の値</returns>
	float GetTriger(Triger type, float deadZone = 0.3f);

	/// <summary>
	/// Stick
	/// </summary>
	/// <param name="type">スティックのタイプ</param>
	/// <param name="deadZone">デッドゾーン(0.0f～1.0f)の範囲内なら0.0fを返す</param>
	/// <returns>-1.0f ～ 1.0f の値</returns>
	float GetStick(Stick type, float deadZone = 0.3f);

	/// <summary>
	/// バイブレーション
	/// </summary>
	/// <param name="leftVibIntensity">左側のバイブレーション 0.0f ～ 1.0f で強さを指定</param>
	/// <param name="rightVibIntensity">右側のバイブレーション 0.0f ～ 1.0f で強さを指定</param>
	void Vibration(float leftVibIntensity, float rightVibIntensity);

	// デバッグ用
	void Debug();
};