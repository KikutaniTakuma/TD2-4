#pragma once

#include <Windows.h>
#undef min
#undef max
#include <cstdint>
#include <string>
#include "Math/Vector2.h"

/// <summary>
/// ウィンドウ管理クラス(ウィンドウ生成等)
/// </summary>
class WindowFactory {
private:
	WindowFactory();
	WindowFactory(const WindowFactory&) = delete;
	WindowFactory(WindowFactory&&) = delete;
	const WindowFactory& operator=(const WindowFactory&) = delete;
	const WindowFactory& operator=(WindowFactory&&) = delete;
public:
	~WindowFactory();

public:
	/// <summary>
	/// シングルトンインスタンス取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static inline WindowFactory* const GetInstance() {
		static WindowFactory instance;
		return &instance;
	}

public:
	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

/// <summary>
/// ウィンドウ
/// </summary>
public:
	/// <summary>
	/// ウィンドウ生成
	/// </summary>
	/// <param name="windowTitle">ウィンドウ名</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="isFullscreen">フルスクリーン化</param>
	void Create(
		const std::wstring& windowTitle = L"LambEngine",
		int32_t width = 1280, 
		int32_t height = 720, 
		bool isFullscreen = false
	);

	/// <summary>
	/// ウィンドウメッセージ
	/// </summary>
	bool WindowMassage();

	/// <summary>
	///  特定の入力でフルスクリーンにする。フルスクリーンなら元のウィンドウに戻す
	/// </summary>
	void Fullscreen();

	/// <summary>
	/// 今このウィンドウがアクティブかどうか
	/// </summary>
	/// <returns>アクティブならtrue</returns>
	bool IsThisWindowaActive() const;

private:
	/// <summary>
	/// ウィンドウのモードを変更
	/// </summary>
	void ChangeWindowMode();


/// <summary>
/// ゲッター
/// </summary>
public:
	/// <summary>
	/// ウィンドウハンドル取得
	/// </summary>
	/// <returns>ウィンドウハンドル</returns>
	inline HWND GetHwnd() const {
		return hwnd_;
	}

	/// <summary>
	/// WNDCLASSEX取得
	/// </summary>
	/// <returns>WNDCLASSEX</returns>
	inline const WNDCLASSEX& GetWNDCLASSEX() const {
		return wndEx_;
	}

	/// <summary>
	/// ウィング名取得
	/// </summary>
	/// <returns></returns>
	inline const std::wstring& GetWindowClassName() const {
		return windowName_;
	}

	/// <summary>
	/// ウィンドウサイズ取得
	/// </summary>
	/// <returns>ウィンドウサイズ</returns>
	Vector2 GetWindowSize() const;

	/// <summary>
	/// クライアント領域取得
	/// </summary>
	/// <returns>クライアント領域</returns>
	const Vector2& GetClientSize() const;

/// <summary>
/// メンバ変数
/// </summary>
private:
	HWND hwnd_ = nullptr;
	WNDCLASSEX wndEx_{};
	UINT windowStyle_;
	RECT windowRect_;
	std::wstring windowName_;

	bool isFullscreen_;
	Vector2 clientSize_;
};