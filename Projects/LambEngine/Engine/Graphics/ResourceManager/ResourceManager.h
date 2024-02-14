#pragma once
#include <unordered_map>
#include <stack>
#include <string>

class ResourceManager {
private:
	ResourceManager() = default;
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
public:
	~ResourceManager() = default;

public:
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

public:
	static void Initialize();

	static void Finalize();

	static ResourceManager* const GetInstance();

private:
	static ResourceManager* instance_;

public:
	void SetTextureResource(const std::string& fileName);
	void SetModelResource(const std::string& fileName);
	void SetAudioResource(const std::string& fileName);

public:
	void Unload();

	void Enable() {
		isEnable_ = true;
	}

	void Unenable() {
		isEnable_ = false;
	}

private:
	std::stack<std::string> resourcesTexture_;
	std::stack<std::string> resourcesModel_;
	std::stack<std::string> resourcesAudio_;

	bool isEnable_ = false;
};