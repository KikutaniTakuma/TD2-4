#include "DsvHeap.h"
#include "Engine/Graphics/DepthBuffer/DepthBuffer.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include <cassert>
#include <algorithm>
#include "Error/Error.h"
#include "Utils/SafeDelete/SafeDelete.h"

DsvHeap* DsvHeap::instance_ = nullptr;

void DsvHeap::Initialize(UINT heapSize) {
	assert(!instance_);
	instance_ = new DsvHeap{ heapSize };
}

void DsvHeap::Finalize() {
	Lamb::SafeDelete(instance_);
}

DsvHeap* const DsvHeap::GetInstance() {
	return instance_;
}

DsvHeap::DsvHeap(uint32_t heapSize) :
	DescriptorHeap{}
{
	CreateDescriptorHeap(heapSize);

	CreateHeapHandles();

	bookingHandle_.clear();

	Lamb::AddLog("Initialize DsvHeap succeeded : heap size is " + std::to_string(heapSize_));
}

void DsvHeap::CreateDescriptorHeap(uint32_t heapSize) {
	heapSize_ = std::clamp(heapSize, DirectXSwapChain::kBackBufferNumber_, 0xffu);
	heap_ = DirectXDevice::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, heapSize_, false);
	heap_.SetName<DsvHeap>();
}

void DsvHeap::CreateHeapHandles() {
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

uint32_t DsvHeap::CreateView(DepthBuffer& depthStencilBuffer) {
	assert(currentHandleIndex_ < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		throw Lamb::Error::Code<DsvHeap>("Over HeapSize", __func__);
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex_);
		depthStencilBuffer.CreateDepthView(heapHandles_[currentHandleIndex_].first, currentHandleIndex_);
		currentHandleIndex_++;
		return currentHandleIndex_ - 1u;
	}
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		depthStencilBuffer.CreateDepthView(heapHandles_[nowCreateViewHandle].first, nowCreateViewHandle);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}