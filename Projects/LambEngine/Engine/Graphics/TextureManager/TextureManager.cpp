#include "TextureManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "../ResourceManager/ResourceManager.h"
#include <cassert>
#include <filesystem>

#include "Error/Error.h"

TextureManager* TextureManager::instance_ = nullptr;

TextureManager* const TextureManager::GetInstance() {
	return instance_;
}

void TextureManager::Initialize() {
	instance_ = new TextureManager();
	instance_->LoadTexture(instance_->white2x2Path_);
}

void TextureManager::Finalize() {
	Lamb::SafeDelete(instance_);
}

TextureManager::TextureManager() :
	textures_(),
	thisFrameLoadFlg_(false),
	threadTextureBuff_(),
	load_(),
	isThreadFinish_(false),
	fence_(),
	fenceVal_(0),
	fenceEvent_(nullptr),
	srvHeap_(nullptr),
	isCloseCommandList_(false),
	isNowThreadLoading_(false),
	exit_(false),
	isThreadLoad_(false),
	white2x2Path_("./Resources/white2x2.png")
{
	ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();

	// コマンドキューを作成
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<TextureManager>("CreateCommandQueue() Failed", __func__);
	}

	// コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<TextureManager>("CreateCommandAllocator() Failed", __func__);
	}

	// コマンドリストを作成する
	commandList_ = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<TextureManager>("CreateCommandList() Failed", __func__);
	}

	// 初期値0でFenceを作る
	fence_ = nullptr;
	fenceVal_ = 0;
	hr = device->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<TextureManager>("CreateFence() Failed", __func__);
	}

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = nullptr;
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (fenceEvent_ == nullptr) {
		throw Lamb::Error::Code<TextureManager>("CreateEvent() Failed", __func__);
	}

	srvHeap_ = CbvSrvUavHeap::GetInstance();

	Lamb::AddLog("Initialize TextureManager succeeded");
}

TextureManager::~TextureManager() {
	if (load_.joinable()) {
		load_.join();
	}
	CloseHandle(fenceEvent_);
	textures_.clear();
}


Texture* const TextureManager::LoadTexture(const std::string& fileName) {
	if (!std::filesystem::exists(fileName)) {
		throw Lamb::Error::Code<TextureManager>("this file is not exist -> " + fileName, __func__);
	}

	auto itr = textures_.find(fileName);
	if (itr == textures_.end()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName, commandList_.Get());

		ExecuteCommandList();
		ResetCommandList();

		if (!tex->isLoad_) {
			return nullptr;
		}

		srvHeap_->CreateView(*tex);

		textures_.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg_ = true;

		ResourceManager::GetInstance()->SetTextureResource(fileName);
	}

	return textures_[fileName].get();
}

Texture* const TextureManager::LoadTexture(const std::string& fileName, ID3D12GraphicsCommandList* const commandList) {
	if (!std::filesystem::exists(fileName)) {
		throw Lamb::Error::Code<TextureManager>("this file is not exist -> " + fileName, __func__);
	}
	auto itr = textures_.find(fileName);
	if (itr == textures_.end()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName, commandList);
		if (!tex->isLoad_) {
			return nullptr;
		}

		srvHeap_->CreateView(*tex);

		textures_.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg_ = true;


		ResourceManager::GetInstance()->SetTextureResource(fileName);
	}

	return textures_[fileName].get();
}

void TextureManager::LoadTexture(const std::string& fileName, Texture** const texPtr) {
	// コンテナに追加
	threadTextureBuff_.push({ fileName, texPtr });
}

void TextureManager::ThreadLoadTexture() {
	if (!load_.joinable() && !threadTextureBuff_.empty()) {
		auto loadProc = [this]() {
			std::unique_lock<std::mutex> uniqueLock(mtx_);

			isNowThreadLoading_ = true;
			while (!threadTextureBuff_.empty()) {
				if (Lamb::IsEngineFianlize()) {
					break;
				}
				auto& front = threadTextureBuff_.front();
				*front.second = LoadTexture(front.first, commandList_.Get());
				threadTextureBuff_.pop();
			}

			isThreadFinish_ = true;
			isNowThreadLoading_ = false;
		};

		load_ = std::thread(loadProc);
	}

	if (isThreadFinish_) {
		ExecuteCommandList();

		if (load_.joinable()) {
			load_.join();
		}

		ResetCommandList();

		isThreadFinish_ = false;
	}
}

void TextureManager::Unload(const std::string& fileName)
{
	auto isExist = textures_.find(fileName);
	if (isExist != textures_.end()) {
		if (textures_[fileName]) {
			textures_[fileName]->Unload();
			textures_[fileName].reset();
		}
		textures_.erase(fileName);
	}
}

Texture* const TextureManager::GetWhiteTex() {
	return textures_[white2x2Path_].get();
}

void TextureManager::ReleaseIntermediateResource() {
	if (thisFrameLoadFlg_ && threadTextureBuff_.empty()) {
		for (auto& i : textures_) {
			i.second->ReleaseIntermediateResource();
		}

		thisFrameLoadFlg_ = false;
	}
}

void TextureManager::ExecuteCommandList()
{
	if (!isCloseCommandList_) {
		isCloseCommandList_ = true;
		/// コマンドリスト
		commandList_->Close();
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(_countof(commandLists), commandLists);
		// Fenceの値を更新
		fenceVal_++;
		// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue_->Signal(fence_.Get(), fenceVal_);

		// Fenceの値が指定したSigna値にたどり着いているか確認する
		// GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence_->GetCompletedValue() < fenceVal_) {
			// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
			fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
			// イベントを待つ
			WaitForSingleObject(fenceEvent_, INFINITE);
		}
		///
	}
}

void TextureManager::ResetCommandList() {
	if (isCloseCommandList_) {
		// 次フレーム用のコマンドリストを準備
		auto hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<TextureManager>("CommandAllocator somthing error", __func__);
		}
		hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<TextureManager>("CommandList somthing error", __func__);
		}
		isCloseCommandList_ = false;
	}
}

void TextureManager::CreateThread()
{
	auto loadProc = [this]() {
		std::unique_lock<std::mutex> uniqueLock(mtx_);
		while (!exit_) {
			condition_.wait(uniqueLock, [this]() { return exit_; });

			if (exit_) {
				break;
			}

			isNowThreadLoading_ = true;
			while (!threadTextureBuff_.empty()) {
				if (Lamb::IsEngineFianlize()) {
					break;
				}
				auto& front = threadTextureBuff_.front();
				*front.second = LoadTexture(front.first, commandList_.Get());
				threadTextureBuff_.pop();
			}

			isThreadFinish_ = true;
			isNowThreadLoading_ = false;
		}
	};

	load_ = std::thread(loadProc);
}

void TextureManager::StartThread()
{
	if (!isThreadLoad_) {
		condition_.notify_all();
		isThreadLoad_ = true;
	}
}

void TextureManager::StopThread()
{
	if (isThreadLoad_) {
		isThreadLoad_ = false;
	}
}

void TextureManager::Use(uint32_t texIndex, UINT rootParam) {
	auto* const mainComlist = DirectXCommand::GetInstance()->GetCommandList();
	mainComlist->SetGraphicsRootDescriptorTable(
		rootParam, srvHeap_->GetGpuHeapHandle(texIndex));
}