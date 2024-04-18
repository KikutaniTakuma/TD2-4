#pragma once
#include <memory>
#include <thread>
#include <queue>

#include "Utils/SafePtr/SafePtr.h"
#include "../GraphicsStructs.h"

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
	static Lamb::SafePtr<MeshManager> instance_;

public:
	[[nodiscard]] Mesh* LoadObj(const std::string& objFileName);

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshs_;
};