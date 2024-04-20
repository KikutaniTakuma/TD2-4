#pragma once
#include "Texture/Texture.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Utils/SafePtr/SafePtr.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <queue>
#include <thread>

/// <summary>
/// テクスチャの管理
/// </summary>
class TextureManager {
public:
	static const std::string kWhiteTexturePath;

private:
	TextureManager();
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) noexcept = delete;

	TextureManager& operator=(const TextureManager&) = delete;
	TextureManager& operator=(TextureManager&&) noexcept = delete;
public:
	~TextureManager();

public:
	static TextureManager* const GetInstance();

	static void Initialize();

	static void Finalize();

private:
	static Lamb::SafePtr<TextureManager> instance_;


public:
	uint32_t LoadTexture(const std::string& fileName);

	const Texture* const GetTexture(const std::string& fileName);

public:
	uint32_t GetWhiteTex();

	void UploadTextureData();

	void ReleaseIntermediateResource();

	void Use(uint32_t texIndex, UINT rootParam);


private:
	class CbvSrvUavHeap* srvHeap_;

	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム  コンテナデータ型: Texture*)
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
	bool thisFrameLoadFlg_;

	std::unique_ptr<DirectXCommand> directXCommand_;
};