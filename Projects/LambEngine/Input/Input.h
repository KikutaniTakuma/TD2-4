#pragma once
#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"
#include <cassert>

/// <summary>
/// 入力関連初期化
/// </summary>
class Input {
private:
	Input();
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	~Input();

	Input& operator=(const Input&) = delete;
	Input& operator=(Input&&) = delete;

public:
	static void Initialize();
	static void Finalize();

	static Input* const GetInstance() {
		assert(instance_);
		return instance_;
	}

private:
	static Input* instance_;

public:
	void InputStart();

private:
	Lamb::LambPtr<IDirectInput8> directInput_;

	Gamepad* gamepad_;
	KeyInput* key_;
	Mouse* mouse_;

public:
	inline Gamepad* const GetGamepad() const {
		return gamepad_;
	}
	inline KeyInput* const GetKey() const {
		return key_;
	}
	inline Mouse* const GetMouse() const {
		return mouse_;
	}
};