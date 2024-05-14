#include "ShaderFactory.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/ConvertString/ConvertString.h"

#include "Error/Error.h"

#include <filesystem>
#include <cassert>
#pragma warning(disable: 6387)

ShaderFactory* ShaderFactory::instance_ = nullptr;

void ShaderFactory::Initialize() {
	assert(!instance_);
	instance_ = new ShaderFactory{};
}

void ShaderFactory::Finalize() {
	assert(!!instance_);
	delete instance_;
	instance_ = nullptr;
}

ShaderFactory* const ShaderFactory::GetInstance() {
	assert(instance_);
	return instance_;
}


ShaderFactory::ShaderFactory():
	dxcUtils_{},
	dxcCompiler_{},
	includeHandler_{}
{
	// dxcCompilerを初期化
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<ShaderFactory>("dxcUtils failed", "DxcCreateInstance", __FILE__, __LINE__);
	}
	else {
		Lamb::AddLog("Create DxcUtils succeeded");
	}

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<ShaderFactory>("dxcCompiler failed", "DxcCreateInstance", __FILE__, __LINE__);
	}
	else {
		Lamb::AddLog("Create IDxcCompiler3 succeeded");
	}

	includeHandler_ = nullptr;
	hr = dxcUtils_->CreateDefaultIncludeHandler(includeHandler_.GetAddressOf());
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<ShaderFactory>("IDxcUtils failed", "CreateDefaultIncludeHandler", __FILE__, __LINE__);
	}
	else {
		Lamb::AddLog("Create IDxcIncludeHandler succeeded");
	}

	Lamb::AddLog("Initialize ShaderFactory succeeded");
}

[[nodiscard]]
IDxcBlob* ShaderFactory::CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile
) {
	if (!std::filesystem::exists(filePath)) {
		throw Lamb::Error::Code<ShaderFactory>("this file is not exists -> " + ConvertString(filePath), ErrorPlace);
	}

	// 1. hlslファイルを読む
	// これからシェーダーをコンパイルする旨をログに出す
	Lamb::AddLog(ConvertString(std::format(L"Begin CompilerShader : path:{} : profile:{}", filePath, profile)));
	// hlslファイルを読む
	Lamb::LambPtr<IDxcBlobEncoding> shaderSource;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, shaderSource.GetAddressOf());
	// 読めなかったら止める
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<ShaderFactory>("Shader Load failed", ErrorPlace);
	}
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	// 2. Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(), // コンパイル対象のhlslファイル名
		L"-E", L"main", // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile, // ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od", // 最適化を外しておく
		L"-Zpr" // メモリレイアウトを優先
	};
	// 実際にShaderをコンパイルする
	Lamb::LambPtr<IDxcResult> shaderResult;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer, // 読みこんだファイル
		arguments,           // コンパイルオプション
		_countof(arguments), // コンパイルオプションの数
		includeHandler_.Get(),      // includeが含まれた諸々
		IID_PPV_ARGS(shaderResult.GetAddressOf()) // コンパイル結果
	);

	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<ShaderFactory>("Danger!! Cannot use ""dxc""", ErrorPlace);
	}

	if (!shaderResult) {
		throw Lamb::Error::Code<ShaderFactory>("Create ShaderResult failed", ErrorPlace);
	}

	// 3. 警告・エラーが出てないか確認する
	Lamb::LambPtr<IDxcBlobUtf8> shaderError;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(shaderError.GetAddressOf()), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		// 警告・エラーダメゼッタイ
		throw Lamb::Error::Code<ShaderFactory>(shaderError->GetStringPointer(), ErrorPlace);
	}

	// 4. Compileを受け取って返す
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<ShaderFactory>("shaderResult->GetOutput() failed", ErrorPlace);
	}
	// 成功したログを出す
	Lamb::AddLog(ConvertString(std::format(L"Compile succeeded : path:{} : profile:{}", filePath, profile)));

	// 実行用バイナリをリターン
	return shaderBlob;
}