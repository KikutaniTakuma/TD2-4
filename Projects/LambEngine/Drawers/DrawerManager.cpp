#include "DrawerManager.h"

Lamb::SafePtr<DrawerManager> DrawerManager::instance_ = nullptr;

DrawerManager::DrawerManager():
	tex2D_(),
	textureManager_(nullptr),
	models_()
{
	textureManager_ = TextureManager::GetInstance();

	tex2D_ = std::make_unique<Texture2D>();
	tex2D_->Load();
}

DrawerManager* DrawerManager::GetInstance()
{
	return instance_.get();
}

void DrawerManager::Initialize() {
	instance_.reset(new DrawerManager());
}

void DrawerManager::Finalize() {
	instance_.reset();
}

Texture2D* const DrawerManager::GetTexture2D() const
{
	return tex2D_.get();
}

uint32_t DrawerManager::LoadTexture(const std::string& fileName)
{
	return textureManager_->LoadTexture(fileName);
}

void DrawerManager::LoadModel(const std::string& fileName) {
	auto isExist = models_.find(fileName);

	if (isExist == models_.end()) {
		std::unique_ptr<Model> newModel = std::make_unique<Model>(fileName);

		models_.insert(std::make_pair(fileName, newModel.release()));
	}
}

Model* const DrawerManager::GetModel(const std::string& fileName)
{
	if (not models_[fileName]) {
		throw Lamb::Error::Code<DrawerManager>("this model is not loaded -> " + fileName, __func__);
	}

	return models_[fileName].get();
}


