#pragma once
#include "Texture/Texture.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

/// <summary>
/// テクスチャの管理
/// </summary>
class TextureManager {
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
	static TextureManager* instance_;


public:
	/// <summary>
	/// テクスチャ読み込み用
	/// </summary>
	/// <param name="fileName">ファイルの名前</param>
	/// <returns>Textureのポインタ(勝手にdeleteしてはいけない)</returns>
	Texture* const LoadTexture(const std::string& fileName);

private:
	/// <summary>
	/// 非同期読み込み用
	/// </summary>
	Texture* const LoadTexture(const std::string& fileName, ID3D12GraphicsCommandList* const commandList_);

public:
	/// <summary>
	/// スレッドを分けてロードする用
	/// </summary>
	/// <param name="fileName"></param>
	/// <param name="texPtr"></param>
	void LoadTexture(const std::string& fileName, Texture** const texPtr);

	/// <summary>
	/// 非同期で画像を読み込む
	/// </summary>
	void ThreadLoadTexture();

public:
	/// <summary>
	/// アンロードする
	/// </summary>
	/// <param name="fileName">アンロードするファイル名</param>
	void Unload(const std::string& fileName);

public:
	/// <summary>
	/// textureを設定する
	/// </summary>
	/// <param name="texIndex">テクスチャのディスクリプタヒープのハンドル</param>
	/// <param name="rootParam">ルートパラメータの</param>
	void Use(uint32_t texIndex, UINT rootParam);

	/// <summary>
	/// CPU側のリソースを解放する
	/// </summary>
	void ReleaseIntermediateResource();

	/// <summary>
	/// 今現在非同期で読み込まれているか
	/// </summary>
	/// <returns>読み込まれていたらtrue</returns>
	bool IsNowThreadLoading() const {
		return isNowThreadLoading_ && !isCloseCommandList_;
	}

	/// <summary>
	/// コマンドリストを取得する
	/// </summary>
	/// <returns>コマンドリスト</returns>
	inline ID3D12GraphicsCommandList* const GetCommandList() const {
		return commandList_.Get();
	}

	/// <summary>
	/// ただの白いテクスチャを取得
	/// </summary>
	/// <returns></returns>
	Texture* const GetWhiteTex();

	/// <summary>
	/// 白いテクスチャのパスを取得
	/// </summary>
	/// <returns></returns>
	const std::string& GetWhiteTexPath() const {
		return white2x2Path_;
	}

private:
	/// <summary>
	/// コマンドリストを実行
	/// </summary>
	void ExecuteCommandList();

	/// <summary>
	/// コマンドリストをリセットする
	/// </summary>
	void ResetCommandList();

	void CreateThread();

	void StartThread();

	void StopThread();

private:
	class CbvSrvUavHeap* srvHeap_;

	Lamb::LambPtr<ID3D12CommandQueue> commandQueue_;
	Lamb::LambPtr<ID3D12CommandAllocator> commandAllocator_;
	Lamb::LambPtr<ID3D12GraphicsCommandList> commandList_;
	bool isCloseCommandList_;

	Lamb::LambPtr<ID3D12Fence> fence_;
	uint64_t fenceVal_;
	HANDLE fenceEvent_;

	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム)
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
	bool thisFrameLoadFlg_;

	std::queue<std::pair<std::string, Texture** const>> threadTextureBuff_;
	std::thread load_;
	std::mutex mtx_;
	std::condition_variable condition_;
	bool exit_;
	bool isThreadLoad_;
	bool isThreadFinish_;
	bool isNowThreadLoading_;


	const std::string white2x2Path_;
};