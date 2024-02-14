#include "StringOutPutManager.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include <cassert>
#include <filesystem>

#include "Error/Error.h"

#include "Utils/SafeDelete/SafeDelete.h"

StringOutPutManager* StringOutPutManager::instance_ = nullptr;

void StringOutPutManager::Initialize() {
	assert(!instance_);
	instance_ = new StringOutPutManager{};
	assert(!!instance_);
}

void StringOutPutManager::Finalize() {
	Lamb::SafeDelete(instance_);
}

StringOutPutManager* const StringOutPutManager::GetInstance() {
	return instance_;
}


StringOutPutManager::StringOutPutManager():
	gmemory_{},
	spriteFonts_{},
	spriteBatch_{}
{
	// GraphicsMemory初期化
	auto device = DirectXDevice::GetInstance()->GetDevice();
	gmemory_.reset(new DirectX::GraphicsMemory(device));
}

void StringOutPutManager::LoadFont(const std::string& fontName) {
	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
	if (!std::filesystem::exists(std::filesystem::path(fontName))) {
		throw Lamb::Error::Code<StringOutPutManager>("This file is not exist -> " + fontName, __func__);
	}

	// もしロード済みなら早期リターン
	auto isLoaded = spriteBatch_.find(fontName);
	if (isLoaded != spriteBatch_.end()) {
		return;
	}

	DirectX::ResourceUploadBatch resUploadBach(device);
	resUploadBach.Begin();
	DirectX::RenderTargetState rtState(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_D32_FLOAT
	);

	DirectX::SpriteBatchPipelineStateDescription pd(rtState);

	// SpriteFontオブジェクトの初期化
	spriteBatch_.insert(
		std::make_pair(fontName, std::make_unique<DirectX::SpriteBatch>(device, resUploadBach, pd)));
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	viewport.Width = clientSize.x;
	viewport.Height = clientSize.y;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	spriteBatch_[fontName]->SetViewport(viewport);

	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	uint32_t useHandle = descriptorHeap->BookingHeapPos(1u);

	spriteFonts_.insert(
		std::make_pair(
			fontName,
			std::make_unique<DirectX::SpriteFont>(
				device,
				resUploadBach,
				ConvertString(fontName).c_str(),
				descriptorHeap->GetCpuHeapHandle(useHandle),
				descriptorHeap->GetGpuHeapHandle(useHandle)
			)
		)
	);

	descriptorHeap->UseThisPosition(useHandle);

	auto directXCommon = DirectXCommand::GetInstance();
	auto future = resUploadBach.End(directXCommon->GetCommandQueue());

	directXCommon->WaitForFinishCommnadlist();

	future.wait();
}

DirectX::SpriteFont* const StringOutPutManager::GetFont(const std::string& fontName) {
	auto spriteFontItr = spriteFonts_.find(fontName);
	if (spriteFontItr != spriteFonts_.end()) {
		return spriteFonts_[fontName].get();
	}
	else {
		return nullptr;
	}
}

DirectX::SpriteBatch* const StringOutPutManager::GetBatch(const std::string& fontName) {
	auto spriteBatchItr = spriteBatch_.find(fontName);
	if (spriteBatchItr != spriteBatch_.end()) {
		return spriteBatch_[fontName].get();
	}
	else {
		return nullptr;
	}
}

void StringOutPutManager::GmemoryCommit() {
	static auto directXCommon = DirectXCommand::GetInstance();
	gmemory_->Commit(directXCommon->GetCommandQueue());
}