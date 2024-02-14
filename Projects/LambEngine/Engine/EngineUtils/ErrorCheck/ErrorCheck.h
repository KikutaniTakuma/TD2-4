#pragma once
#include <string>

/// <summary>
/// エラーのチェック(ErrorTextBox()を呼び出したらメインループが止まる)
/// </summary>
class ErrorCheck {
private:
	ErrorCheck();
	ErrorCheck(const ErrorCheck&) = delete;
	ErrorCheck(ErrorCheck&&) noexcept = delete;
	~ErrorCheck() = default;

	ErrorCheck& operator=(const ErrorCheck&) = delete;
	ErrorCheck& operator=(ErrorCheck&&) noexcept = delete;

public:
	/// <summary>
	/// シングルトン
	/// </summary>
	/// <returns>インスタンス</returns>
	static ErrorCheck* const GetInstance();

public:
	/// <summary>
	/// エラーログ
	/// </summary>
	/// <param name="text">エラーメッセージ</param>
	/// <param name="boxName">エラーウィンドウの名前</param>
	void ErrorTextBox(const std::string& text, const std::string& boxName = "Error");

	/// <summary>
	/// エラーが起きているかの判定
	/// </summary>
	/// <returns></returns>
	inline bool GetError() const {
		return isError_;
	}
	void CrashProgram();

private:
	void ErrorLog(const std::string& text, const std::string& boxName = "Error");


private:
	bool isError_;
};