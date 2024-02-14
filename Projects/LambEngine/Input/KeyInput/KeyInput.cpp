#include "KeyInput.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include <cassert>
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Error/Error.h"

void KeyInput::Input() {
	if (!initalizeSucceeded_) {
		return;
	}

	std::copy(key_.begin(), key_.end(), preKey_.begin());
	
	// キーボード情報取得開始
	keyBoard_->Acquire();

	// キー入力
	key_ = { 0 };
	keyBoard_->GetDeviceState(DWORD(key_.size()), key_.data());
}

void KeyInput::InputReset() {
	key_ = { 0 };
	std::copy(key_.begin(), key_.end(), preKey_.begin());
}

bool KeyInput::Pushed(uint8_t keyType) {
	if (!initalizeSucceeded_) {
		return false;
	}
	return (key_[keyType] & 0x80) && !(preKey_[keyType] & 0x80);
}
bool KeyInput::LongPush(uint8_t keyType) {
	if (!initalizeSucceeded_) {
		return false;
	}
	return (key_[keyType] & 0x80) && (preKey_[keyType] & 0x80);
}
bool KeyInput::Released(uint8_t keyType) {
	if (!initalizeSucceeded_) {
		return false;
	}
	return !(key_[keyType] & 0x80) && (preKey_[keyType] & 0x80);
}

bool KeyInput::PushAnyKey() {
	return key_ != preKey_;
}

KeyInput* KeyInput::instance_ = nullptr;

void KeyInput::Initialize(IDirectInput8* input) {
	assert(input);
	instance_ = new KeyInput(input);
}
void KeyInput::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

KeyInput::KeyInput(IDirectInput8* input):
	keyBoard_(),
	key_{0},
	preKey_{0},
	initalizeSucceeded_(false)
{
	
	HRESULT hr = input->CreateDevice(GUID_SysKeyboard, keyBoard_.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<KeyInput>("CreateDevice failed", "Constructor");
	}

	hr = keyBoard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<KeyInput>("SetDataFormat failed", "Constructor");
	}

	hr = keyBoard_->SetCooperativeLevel(WindowFactory::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<KeyInput>("SetDataFormat failed", "Constructor");
	}

	initalizeSucceeded_ = true;

	Lamb::AddLog("Initialize KeyInput succeeded");
}

KeyInput::~KeyInput() {
	keyBoard_->Unacquire();
}