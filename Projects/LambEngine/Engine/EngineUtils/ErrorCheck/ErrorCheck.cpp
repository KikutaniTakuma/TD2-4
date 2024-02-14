#include "ErrorCheck.h"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <format>
#include <cassert>
#include <cstdlib>
#include <Windows.h>
#undef max
#undef min
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Engine.h"
#include <typeinfo>

ErrorCheck* const ErrorCheck::GetInstance() {
	static ErrorCheck instance;
	return &instance;
}

ErrorCheck::ErrorCheck() :
	isError_(false)
{}

void ErrorCheck::ErrorTextBox(const std::string& text, const std::string& boxName) {
	ErrorLog(text, boxName);

	if (boxName == "Error") {
		MessageBoxA(
			WindowFactory::GetInstance()->GetHwnd(), 
			text.c_str(), boxName.c_str(), 
			MB_OK | MB_APPLMODAL | MB_ICONERROR
		);
	}
	else {
		MessageBoxA(
			WindowFactory::GetInstance()->GetHwnd(), 
			text.c_str(), ("Error : " + boxName).c_str(),
			MB_OK | MB_APPLMODAL | MB_ICONERROR
		);
	}
	isError_ = true;
}

void ErrorCheck::CrashProgram() {
	if (isError_) {
		Lamb::AddLog("WARNIG CrashProgram : Please check the Error.log");

		std::exit(EXIT_FAILURE);
	}
}

void ErrorCheck::ErrorLog(const std::string& text, const std::string& boxName) {
	std::filesystem::path directoryPath = "./Log/";
	if (!std::filesystem::exists(directoryPath)) {
		std::filesystem::create_directory(directoryPath);
	}
	std::ofstream file;
	try {
		file.open(directoryPath.string() + "Error.log", std::ios::app);
	}
	catch (const std::exception& err) {
		MessageBoxA(
			WindowFactory::GetInstance()->GetHwnd(),
			err.what(), ("Error : " + std::string{ typeid(ErrorCheck).name() }).c_str(),
			MB_OK | MB_SYSTEMMODAL | MB_ICONERROR
		);

		return;
	}

	file << Lamb::NowTime() << ": "  << std::format("{} : {}", boxName, text) << std::endl;
	file.close();
}