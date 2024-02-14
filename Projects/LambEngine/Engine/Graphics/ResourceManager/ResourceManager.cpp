#include "ResourceManager.h"
#include "Error/Error.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "AudioManager/AudioManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::Initialize()
{
	if (!instance_) {
		instance_ = new ResourceManager{};
		if (!instance_) {
			throw Lamb::Error::Code<ResourceManager>("new failed", __func__);
		}
	}
}

void ResourceManager::Finalize()
{
	if (instance_) {
		Lamb::SafeDelete(instance_);
	}
}

ResourceManager* const ResourceManager::GetInstance()
{
	return instance_;
}

void ResourceManager::SetTextureResource(const std::string& fileName)
{
	if (isEnable_) {
		if (fileName == TextureManager::GetInstance()->GetWhiteTexPath()) {
			return;
		}
		resourcesTexture_.push(fileName);
	}
}

void ResourceManager::SetModelResource(const std::string& fileName)
{
	if (isEnable_) {
		resourcesModel_.push(fileName);
	}
}

void ResourceManager::SetAudioResource(const std::string& fileName)
{
	if (isEnable_) {
		resourcesAudio_.push(fileName);
	}
}

void ResourceManager::Unload()
{
	TextureManager* const textureManager = TextureManager::GetInstance();
	MeshManager* const meshManager = MeshManager::GetInstance();
	AudioManager* const audioManager = AudioManager::GetInstance();

	while (!resourcesTexture_.empty()) {
		textureManager->Unload(resourcesTexture_.top());
		resourcesTexture_.pop();
	}
	
	while (!resourcesModel_.empty()) {
		meshManager->Unload(resourcesModel_.top());
		resourcesModel_.pop();
	}

	while (!resourcesAudio_.empty()) {
		audioManager->Unload(resourcesAudio_.top());
		resourcesAudio_.pop();
	}
}
