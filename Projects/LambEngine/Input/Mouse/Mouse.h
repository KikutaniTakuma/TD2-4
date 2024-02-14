#pragma once
#include <array>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "Math/Vector2.h"
#include "Engine/EngineUtils/LambPtr/LambPtr.h"

/// <summary>
/// マウスの入力
/// </summary>
class Mouse {
public:
	enum class Button : uint8_t {
		Left = 0,
		Right,
		Middle,
		EX0,
		EX1,
		EX2,
		EX3,
		EX4
	};


private:
	Mouse() = delete;
	Mouse(IDirectInput8* input);
	Mouse(const Mouse&) = delete;
	Mouse(Mouse&&) = delete;
	~Mouse();

	Mouse& operator=(const Mouse&) = delete;
	Mouse& operator=(Mouse&&) = delete;

public:
	 void Input();
	 void InputReset();

	 bool Pushed(Mouse::Button button);
	 bool LongPush(Mouse::Button button);
	 bool Releaed(Mouse::Button button);

	/// <summary>
	/// 何かしらのキーが押された、またはマウスが動いた、ホイールを動かした
	/// </summary>
	/// <returns>押されたらtrue</returns>
	 bool PushAnyKey();

	 Vector2 GetVelocity();

	 float GetWheel();
	 float GetWheelVelocity();

	 Vector2 GetPos();

	 void Show(bool flg);

public:
	static void Initialize(IDirectInput8* input);
	static void Finalize();

	void Debug();

private:
	static Mouse* instance_;

public:
	static Mouse* const GetInstance() {
		return instance_;
	}

private:
	Lamb::LambPtr<IDirectInputDevice8> mouse_;

	DIMOUSESTATE2 mosueState_;
	DIMOUSESTATE2 preMosueState_;

	size_t wheel_;

	bool initalizeSucceeded_;
};