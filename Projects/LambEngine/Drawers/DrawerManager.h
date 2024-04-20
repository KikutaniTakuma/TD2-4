#pragma once
#include <unordered_map>
#include <memory>
#include "Texture2D/Texture2D.h"
#include "Model/Model.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

class DrawerManager {
private:
	DrawerManager();
	DrawerManager(const DrawerManager&) = delete;
	DrawerManager(DrawerManager&&) = delete;

	DrawerManager& operator=(const DrawerManager&) = delete;
	DrawerManager& operator=(DrawerManager&&) = delete;
public:
	~DrawerManager() = default;

public:
	static DrawerManager* GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static Lamb::SafePtr<DrawerManager> instance_;

public:
	Texture2D* const GetTexture2D() const;
	uint32_t LoadTexture(const std::string& fileName);

private:
	std::unique_ptr<Texture2D> tex2D_;
	Lamb::SafePtr<TextureManager> textureManager_;


public:
	void LoadModel(const std::string& fileName);
	Model* const GetModel(const std::string& fileName);

private:
	std::unordered_map<std::string, std::unique_ptr<Model>> models_;
};