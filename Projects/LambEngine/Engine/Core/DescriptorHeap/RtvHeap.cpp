#include "RtvHeap.h"
#include "Engine/Graphics/RenderTarget/RenderTarget.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Engine.h"
#include "Math/Vector4.h"
#include <algorithm>
#include <cassert>
#include "Error/Error.h"

#include "Utils/SafeDelete/SafeDelete.h"

RtvHeap* RtvHeap::instance_ = nullptr;

void RtvHeap::Initialize(UINT heapSize) {
	assert(!instance_);
	instance_ = new RtvHeap{ heapSize };
}

void RtvHeap::Finalize() {
	Lamb::SafeDelete(instance_);
}

RtvHeap* const RtvHeap::GetInstance() {
	return instance_;
}

RtvHeap::RtvHeap(uint32_t heapSize):
	DescriptorHeap{}
{
	CreateDescriptorHeap(heapSize);

	CreateHeapHandles();

	bookingHandle_.clear();

	Lamb::AddLog("Initialize RtvHeap succeeded : heap size is " + std::to_string(heapSize_));
}

void RtvHeap::CreateDescriptorHeap(uint32_t heapSize) {
	heapSize_ = std::clamp(heapSize, DirectXSwapChain::kBackBufferNumber_, 0xffu);
	heap_ = DirectXDevice::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, heapSize_, false);
	heap_.SetName<RtvHeap>();
}

void RtvHeap::CreateHeapHandles() {
	UINT incrementSRVCBVUAVHeap = DirectXDevice::GetInstance()->GetIncrementSRVCBVUAVHeap();

	heapHandles_.reserve(heapSize_);
	heapHandles_.push_back({ heap_->GetCPUDescriptorHandleForHeapStart(),
							D3D12_GPU_DESCRIPTOR_HANDLE{} });
	auto heapHandleFirstItr = heapHandles_.begin();
	for (uint32_t i = 1; i < heapSize_; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += incrementSRVCBVUAVHeap * i;
		heapHandles_.push_back(hadleTmp);
	}
}

void RtvHeap::CreateBackBuffer(
	std::array<Lamb::LambPtr<ID3D12Resource>, DirectXSwapChain::kBackBufferNumber_>& backBuffer,
	IDXGISwapChain4* const swapChain
) {
	assert(!!swapChain);
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (uint32_t i = 0u; i < DirectXSwapChain::kBackBufferNumber_; i++) {
		HRESULT hr = swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer[i].GetAddressOf()));

		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<RtvHeap>("GetBuffer() Failed", __func__);
		}

		device->CreateRenderTargetView(backBuffer[i].Get(), &rtvDesc, heapHandles_[i].first);

		useHandle_.push_back(i);
		currentHandleIndex_++;
	}

	Lamb::AddLog(std::string{ __func__ } + " succeeded");
}

void RtvHeap::SetMainRtv() {
	IDXGISwapChain4* const swapChain = DirectXSwapChain::GetInstance()->GetSwapChain();
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	// 描画先をRTVを設定する
	auto dsvH = Engine::GetDsvHandle();
	commandList->OMSetRenderTargets(1, &heapHandles_[backBufferIndex].first, false, &dsvH);
}

void RtvHeap::SetRtv(uint32_t heapHandle, uint32_t renderTargetNum, bool isDrawDepth) {
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();
	// 描画先をRTVを設定する
	auto dsvH = Engine::GetDsvHandle();
	commandList->OMSetRenderTargets(renderTargetNum, &heapHandles_[heapHandle].first, false, isDrawDepth ? &dsvH : nullptr);
}
void RtvHeap::SetRtv(std::initializer_list<D3D12_CPU_DESCRIPTOR_HANDLE> heapHandles, bool isDrawDepth) {
	assert(0llu < heapHandles.size() || heapHandles.size() <= 8llu);
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
	for (auto& i : heapHandles) {
		handles.push_back(i);
	}

	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();
	// 描画先をRTVを設定する
	auto dsvH = Engine::GetDsvHandle();
	commandList->OMSetRenderTargets(static_cast<uint32_t>(handles.size()), &handles[0], false, isDrawDepth ? &dsvH : nullptr);
}

void RtvHeap::ClearRenderTargetView(uint32_t handle, const Vector4& clearColor) {
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();
	commandList->ClearRenderTargetView(heapHandles_[handle].first, clearColor.m.data(), 0, nullptr);
}

uint32_t RtvHeap::CreateView(class RenderTarget& peraRender) {
	assert(currentHandleIndex_ < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		throw Lamb::Error::Code<RtvHeap>("Over HeapSize", __func__);
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex_);
		peraRender.CreateRTV(heapHandles_[currentHandleIndex_].first, currentHandleIndex_);
		currentHandleIndex_++;
		return currentHandleIndex_ - 1u;
	}
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		peraRender.CreateRTV(heapHandles_[nowCreateViewHandle].first, nowCreateViewHandle);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}
