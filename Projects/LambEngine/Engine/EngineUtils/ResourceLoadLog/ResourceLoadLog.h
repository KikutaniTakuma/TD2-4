#pragma once
#include <list>
#include <filesystem>

class ResourceLoadLog {
private:
	ResourceLoadLog() = default;
	ResourceLoadLog(const ResourceLoadLog&) = delete;
	ResourceLoadLog(ResourceLoadLog&&) = delete;
	
public:
	~ResourceLoadLog();


public:
	static ResourceLoadLog* GetInstance();

	static void Set(const std::filesystem::path& path);


private:
	std::list<std::filesystem::path> resoucePathData;
};