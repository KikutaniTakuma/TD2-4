#include "MeshManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "../ResourceManager/ResourceManager.h"

MeshManager* MeshManager::instance_ = nullptr;

MeshManager* const MeshManager::GetInstance() {
	assert(instance_);
	return instance_;
}

MeshManager::~MeshManager() {
	if (load_.joinable()) {
		load_.join();
	}

	meshs_.clear();

	while (!threadMeshBuff_.empty()) {
		threadMeshBuff_.pop();
	}
}

void MeshManager::Initialize() {
	instance_ = new MeshManager{};
	if (instance_) {
		Lamb::AddLog("Initialize MeshManager succeeded");
	}
	else {
		throw Lamb::Error::Code<MeshManager>("instance is nullptr", __func__);
	}
}
void MeshManager::Finalize() {
	Lamb::SafeDelete(instance_);
}

Mesh* MeshManager::LoadObj(const std::string& objFileName) {
	auto mesh = meshs_.find(objFileName);

	if (mesh == meshs_.end()) {
		meshs_[objFileName];
		meshs_[objFileName] = std::make_unique<Mesh>();
		meshs_[objFileName]->LoadObj(objFileName);
		
		ResourceManager::GetInstance()->SetModelResource(objFileName);
	}

	return meshs_[objFileName].get();
}

void MeshManager::LoadObj(const std::string& objFileName, Mesh**const mesh) {
	threadMeshBuff_.push(std::make_pair(objFileName, mesh));
}

void MeshManager::Unload(const std::string& objFileName)
{
	auto isExist = meshs_.find(objFileName);
	if (isExist != meshs_.end()) {
		if (meshs_[objFileName]) {
			meshs_[objFileName].reset();
		}
		meshs_.erase(objFileName);
	}
}

void MeshManager::ResetDrawCount() {
	for (auto& i : meshs_) {
		i.second->ResetDrawCount();
	}
}

void MeshManager::Draw() {
	for (auto& i : meshs_) {
		i.second->Draw();
	}
}

void MeshManager::ThreadLoad() {
	// 読み込み予定の物があるかつ今読み込み中ではない
	if (!threadMeshBuff_.empty() && !load_.joinable()) {
		auto loadProc = [this]() {
			std::unique_lock<std::mutex> uniqueLock(mtx_);

			isNowThreadLoading_ = true;
			while (!threadMeshBuff_.empty()) {
				if (Lamb::IsEngineFianlize()) {
					break;
				}
				// ロードするmeshを取得
				auto& front = threadMeshBuff_.front();
				
				// ロード済みか判定
				auto mesh = meshs_.find(front.first);

				// ロード済みではなかったらロードして代入
				if (mesh == meshs_.end()) {
					meshs_[front.first] = std::make_unique<Mesh>();
					meshs_[front.first]->ThreadLoadObj(front.first);
					ResourceManager::GetInstance()->SetTextureResource(front.first);
					if (Lamb::IsEngineFianlize()) {
						break;
					}
					(*front.second) = meshs_[front.first].get();
				}
				// ロード済みだったらロード済みのmeshポインタを代入
				else {
					if (Lamb::IsEngineFianlize()) {
						break;
					}
					(*front.second) = mesh->second.get();
				}
				threadMeshBuff_.pop();
			}

			// テクスチャの読み込みが終わるまでループ
			bool isTextureLoadFinish = false;
			while (!isTextureLoadFinish) {
				if (Lamb::IsEngineFianlize()) {
					break;
				}

				for (auto& i : meshs_) {
					i.second->CheckModelTextureLoadFinish();
				}
				for (auto& i : meshs_) {
					if (!i.second->isLoad_) {
						isTextureLoadFinish = false;
						break;
					}
					else {
						isTextureLoadFinish = true;
					}
				}
			}

			isThreadFinish_ = true;
			isNowThreadLoading_ = false;
			};

		// 非同期開始
		load_ = std::thread(loadProc);
	}
}

void MeshManager::CheckLoadFinish() {
	if (isThreadFinish_) {
		for (auto& i : meshs_) {
			if (!i.second->isLoad_) {
				return;
			}
		}

		isThreadFinish_ = false;
	}
}
void MeshManager::JoinThread() {
	if (isThreadFinish_ && load_.joinable()) {
		load_.join();
	}
}

bool MeshManager::IsNowThreadLoading() const {
	return isNowThreadLoading_;
}