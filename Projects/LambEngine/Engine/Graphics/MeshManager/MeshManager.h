#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include "Mesh/Mesh.h"
#include "Utils/UtilsLib/UtilsLib.h"

/// <summary>
/// メッシュを管理
/// </summary>
class MeshManager final {
private:
	MeshManager() = default;
	MeshManager(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;

	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;
public:
	~MeshManager();

public:
	static MeshManager* const GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static MeshManager* instance_;

public:
	/// <summary>
	/// Objファイル読み込み
	/// </summary>
	/// <param name="objFileName">ファイルの名前</param>
	/// <returns>読み込んだメッシュ</returns>
	Mesh* LoadObj(const std::string& objFileName);

	/// <summary>
	/// 非同期読み込み(この関数単体では非同期で読み込まない)
	/// </summary>
	/// <param name="objFileName">ファイルの名前</param>
	/// <param name="mesh">読み込むMeshのダブルポインタ</param>
	void LoadObj(const std::string& objFileName, Mesh**const mesh);

public:
	/// <summary>
	/// アンロードする
	/// </summary>
	/// <param name="objFileName">ファイルの名前</param>
	void Unload(const std::string& objFileName);

public:
	void ResetDrawCount();

	void Draw();



public:
	void ThreadLoad();

	void CheckLoadFinish();

	void JoinThread();

	bool IsNowThreadLoading() const;

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshs_;

	std::queue<std::pair<std::string, Mesh** const>> threadMeshBuff_;
	std::thread load_;
	std::mutex mtx_;
	bool isThreadFinish_;
	bool isNowThreadLoading_;
};