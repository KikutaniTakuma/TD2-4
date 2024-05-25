#include "ResourceLoadLog.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/SafePtr/SafePtr.h"
#include <fstream>
#include <algorithm>


ResourceLoadLog::~ResourceLoadLog() {
	std::ofstream file;
	file.open("./Log/LoadResrouce.log");

	for (auto& i : resoucePathData) {
		file << i.string() << std::endl;
	}

	file.close();
}

ResourceLoadLog* ResourceLoadLog::GetInstance()
{
	static ResourceLoadLog instance_;
	return &instance_;
}

void ResourceLoadLog::Set([[maybe_unused]]const std::filesystem::path& path) {
	Lamb::SafePtr instance = GetInstance();
	auto&& key = (path.parent_path() / path.stem()).generic_wstring();
	auto&& i = key;
	i;
	if (std::find_if(
		instance->resoucePathData.begin(),
		instance->resoucePathData.end(),
		[&key](const std::filesystem::path& path)->bool {
			return key == path.generic_wstring();
		}) == instance->resoucePathData.end()
	) {
		instance->resoucePathData.push_back(key);
	}
}
