#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "../externals/DirectXTK12/Inc/SpriteFont.h"
#include "../externals/DirectXTK12/Inc/ResourceUploadBatch.h"
#pragma comment(lib, "DirectXTK12.lib")

class StringOutPutManager {
private:
	StringOutPutManager();
	StringOutPutManager(const StringOutPutManager&) = delete;
	StringOutPutManager(StringOutPutManager&) = delete;

	StringOutPutManager& operator=(const StringOutPutManager&) = delete;
	StringOutPutManager& operator=(StringOutPutManager&&) = delete;
public:
	~StringOutPutManager() = default;

public:
	static void Initialize();

	static void Finalize();

	static StringOutPutManager* const GetInstance();

private:
	static StringOutPutManager* instance_;

public:
	void LoadFont(const std::string& fontName);

	DirectX::SpriteFont* const GetFont(const std::string& fontName);

	DirectX::SpriteBatch* const GetBatch(const std::string& fontName);

	void GmemoryCommit();

private:
	std::unique_ptr<DirectX::GraphicsMemory> gmemory_;
	std::unordered_map<std::string, std::unique_ptr<DirectX::SpriteFont>> spriteFonts_;
	std::unordered_map<std::string, std::unique_ptr<DirectX::SpriteBatch>> spriteBatch_;
};