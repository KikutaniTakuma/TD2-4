#include "CbvSrvUavHeap.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Graphics/DepthBuffer/DepthBuffer.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <numeric>
#include "Utils/SafeDelete/SafeDelete.h"

#include "Engine/Core/DescriptorHeap/Descriptor.h"

CbvSrvUavHeap* CbvSrvUavHeap::instance_ = nullptr;

void CbvSrvUavHeap::Initialize(UINT heapSize) {
	instance_ = new CbvSrvUavHeap{ heapSize };
}

void CbvSrvUavHeap::Finalize() {
	Lamb::SafeDelete(instance_);
}

CbvSrvUavHeap* const CbvSrvUavHeap::GetInstance() {
	return instance_;
}

CbvSrvUavHeap::CbvSrvUavHeap(UINT numDescriptor) :
	DescriptorHeap{}
{
	CreateDescriptorHeap(numDescriptor);

	CreateHeapHandles();

	bookingHandle_.clear();

	Lamb::AddLog("Initialize CbvSrvUavHeap succeeded : heap size is " + std::to_string(heapSize_));
}

void CbvSrvUavHeap::CreateDescriptorHeap(uint32_t heapSize) {
	// 1～(10^6-1)でクランプ
	heapSize_ = std::clamp(heapSize, 1u, static_cast<UINT>(std::pow(10u, 6u)) - 1u);
	heap_ = DirectXDevice::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, heapSize_, true);
	heap_.SetName<CbvSrvUavHeap>();
}

void CbvSrvUavHeap::SetHeap() {
	static auto commandlist = DirectXCommand::GetInstance()->GetCommandList();
	commandlist->SetDescriptorHeaps(1, heap_.GetAddressOf());
}
void CbvSrvUavHeap::Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex) {
	static auto commandlist = DirectXCommand::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, handle);
}
void CbvSrvUavHeap::Use(uint32_t handleIndex, UINT rootParmIndex) {
	auto commandlist = DirectXCommand::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, heapHandles_[handleIndex].second);
}

uint32_t CbvSrvUavHeap::CreateView(Descriptor& buffer) {
	if (currentHandleIndex_ >= heapSize_) {
		throw Lamb::Error::Code<CbvSrvUavHeap>("Over HeapSize", __func__);
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex_);
		buffer.CreateView(heapHandles_[currentHandleIndex_].first, heapHandles_[currentHandleIndex_].second, currentHandleIndex_);
		currentHandleIndex_++;
		return currentHandleIndex_ - 1u;
	}
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		buffer.CreateView(heapHandles_[nowCreateViewHandle].first, heapHandles_[nowCreateViewHandle].second, nowCreateViewHandle);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}