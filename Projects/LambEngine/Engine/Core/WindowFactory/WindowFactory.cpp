#include "WindowFactory.h"
#pragma comment(lib, "winmm.lib")

#include "imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <cassert>

#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/EngineUtils/ErrorCheck/ErrorCheck.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Input/Input.h"

#include "Error/Error.h"

#include "Utils/ExecutionLog/ExecutionLog.h"

WindowFactory::WindowFactory():
	hwnd_{},
	wndEx_{},
	windowStyle_(0u),
	windowRect_{},
	windowName_(),
	isFullscreen_(false),
	clientSize_{}
{
	timeBeginPeriod(1);
}

WindowFactory::~WindowFactory() {
	UnregisterClass(wndEx_.lpszClassName, wndEx_.hInstance);
}

LRESULT WindowFactory::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif // _DEBUG

	switch (msg) {
	case WM_DESTROY:        // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}

void WindowFactory::Create(
	const std::wstring& windowTitle, 
	int32_t width,
	int32_t height, 
	bool isFullscreen
) {
	windowName_ = windowTitle;

	// 最大化ボタンを持たないかつサイズ変更不可
	windowStyle_ = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

	wndEx_.cbSize = sizeof(WNDCLASSEX);
	wndEx_.lpfnWndProc = WindowProcedure;
	wndEx_.lpszClassName = windowTitle.c_str();
	wndEx_.hInstance = GetModuleHandle(nullptr);
	wndEx_.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndEx_);

	clientSize_ = {static_cast<float>(width),static_cast<float>(height)};

	windowRect_ = { 0,0,width, height };

	// 指定のサイズになるようなウィンドウサイズを計算
	AdjustWindowRect(&windowRect_, windowStyle_, false);

	hwnd_ = CreateWindow(
		wndEx_.lpszClassName,
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect_.right - windowRect_.left,
		windowRect_.bottom - windowRect_.top,
		nullptr,
		nullptr,
		wndEx_.hInstance,
		nullptr
	);

	windowStyle_ &= ~WS_THICKFRAME;

	SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);
	SetWindowPos(
		hwnd_, NULL, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED));
	ShowWindow(hwnd_, SW_NORMAL);

	isFullscreen_ = isFullscreen;

	if (isFullscreen_) {
		ChangeWindowMode();
	}

	Lamb::AddLog("Create Window succeeded");
	Lamb::AddLog(std::string{ "Window name : " } + ConvertString(windowTitle));
	Lamb::AddLog("size : " + std::to_string(width) + ", " + std::to_string(height));
	Lamb::AddLog("window mode : " + (isFullscreen ? std::string{ "fullscreen" } : std::string{ "windowed" }));
}

bool WindowFactory::WindowMassage() {
	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	static ErrorCheck* const err = ErrorCheck::GetInstance();

	return (msg.message != WM_QUIT) && !(err->GetError());
}

void WindowFactory::ChangeWindowMode() {
	if (isFullscreen_) {
		// 通常ウィンドウの時の状態を保存
		windowStyle_ = GetWindowLong(hwnd_, GWL_STYLE);
		GetWindowRect(hwnd_, &windowRect_);

		// フルスクリーンにする
		SetWindowLong(hwnd_, GWL_STYLE, WS_POPUP);
		SetWindowPos(hwnd_, HWND_TOP, 0, 0, 
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN), 
			SWP_FRAMECHANGED | SWP_SHOWWINDOW
		);
	}
	else {
		// 通常のウィンドウに戻す
		SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);
		SetWindowPos(
			hwnd_, 
			NULL,
			windowRect_.left, windowRect_.top,
			windowRect_.right - windowRect_.left, 
			windowRect_.bottom - windowRect_.top,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW
		);
	}
}

void WindowFactory::Fullscreen() {
	static KeyInput* const key = Input::GetInstance()->GetKey();
	assert(!!key);
	if (key->Pushed(DIK_F11) || 
		((key->LongPush(DIK_LALT) || key->LongPush(DIK_RALT)) && key->Pushed(DIK_RETURN))
		) 
	{
		isFullscreen_ = !isFullscreen_;
		ChangeWindowMode();
	}
}

bool WindowFactory::IsThisWindowaActive() const {
	HWND currentActiveWindow = GetForegroundWindow();

	return hwnd_ == currentActiveWindow;
}


Vector2 WindowFactory::GetWindowSize() const {
	return Vector2(
		static_cast<float>(windowRect_.right), 
		static_cast<float>(windowRect_.bottom)
	);
}

const Vector2& WindowFactory::GetClientSize() const {
	return clientSize_;
}