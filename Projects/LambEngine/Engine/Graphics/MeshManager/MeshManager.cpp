#include "MeshManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "../MeshLoader/MeshLoader.h"

Lamb::SafePtr<MeshManager> MeshManager::instance_ = nullptr;

MeshManager* const MeshManager::GetInstance() {
	return instance_.get();
}

MeshManager::~MeshManager() {
	meshs_.clear();
}

void MeshManager::Initialize() {
	instance_.reset(new MeshManager());
	if (instance_) {
		Lamb::AddLog("Initialize MeshManager succeeded");
	}
	else {
		throw Lamb::Error::Code<MeshManager>("instance is nullptr", ErrorPlace);
	}
}
void MeshManager::Finalize() {
	instance_.reset();
}

Mesh* MeshManager::LoadObj(const std::string& objFileName) {
	auto mesh = meshs_.find(objFileName);

	if (mesh == meshs_.end()) {
		meshs_[objFileName];
		meshs_[objFileName] = std::make_unique<Mesh>();
		*meshs_[objFileName] = MeshLoader::LoadModel(objFileName);
	}

	return meshs_[objFileName].get();
}