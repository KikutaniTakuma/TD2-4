#pragma once
#include <string>
#include <format>
#include "Utils/Cocepts/Cocepts.h"

#include <Windows.h>
#undef max
#undef min

class Vector2;
class Vector3;
class Vector4;
class Quaternion;


/// <summary>
/// ログ関連
/// </summary>
namespace Lamb {
	/// <summary>
	/// 実行ログをファイルに出力する
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <returns>成功したか否か</returns>
	bool AddLog(const std::string& text);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	void DebugLog(const std::string& text);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">vector</param>
	void DebugLog(const std::string& text, const Vector2& vec);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">vector</param>
	void DebugLog(const std::string& text, const Vector3& vec);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">vector</param>
	void DebugLog(const std::string& text, const Vector4& vec);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">quaternion</param>
	void DebugLog(const std::string& text, const Quaternion& quaternion);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">vector</param>
	template<Lamb::IsNumber T>
	void DebugLog(const std::string& text, const T& number) {
		OutputDebugStringA((text + std::string{ " : " } + std::format("{}", number) + "\n").c_str());
	}

	/// <summary>
	/// ErrorCheck::ErrorTextBoxのラッパー関数
	/// </summary>
	/// <param name="erroerMassage">エラーメッセージ</param>
	/// <param name="functionName">エラーが起きた関数名</param>
	/// <param name="className">エラーが起きたクラス名</param>
	void ErrorLog(
		const std::string& erroerMassage,
		const std::string& functionName = std::string{},
		const std::string& className = "Error"
		);

	void ErrorLog(const class Error& err);

	/// <summary>
	/// 今の時間を文字列として出力する
	/// </summary>
	/// <returns>今の時間(jst)</returns>
	std::string NowTime();
}