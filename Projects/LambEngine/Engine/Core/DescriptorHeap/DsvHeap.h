#pragma once
#include "Engine/Core/DescriptorHeap/DescriptorHeap.h"

class DsvHeap final : public DescriptorHeap{
private:
	DsvHeap(uint32_t heapSize);

	DsvHeap() = delete;
	DsvHeap(const DsvHeap&) = delete;
	DsvHeap(DsvHeap&&) = delete;
	DsvHeap& operator=(const DsvHeap&) = delete;
	DsvHeap& operator=(DsvHeap&&) = delete;

public:
	~DsvHeap() = default;

public:
	static void Initialize(UINT heapSize);

	static void Finalize();

	static DsvHeap* const GetInstance();

private:
	static DsvHeap* instance_;

private:
	void CreateDescriptorHeap(uint32_t heapSize) override;

	void CreateHeapHandles() override;

	[[deprecated("Don`t use this function")]]
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHeapHandle([[maybe_unused]] uint32_t heapIndex) override {
		return D3D12_GPU_DESCRIPTOR_HANDLE{};
	}

public:
	uint32_t CreateView(class DepthBuffer& depthStencilBuffer);
};