#pragma once
#include "DescriptorHeap.h"
#include <array>
#include <initializer_list>
#include "Utils/SafePtr/SafePtr.h"

class RtvHeap final : public DescriptorHeap {
public:
	RtvHeap() = delete;
	RtvHeap(const RtvHeap&) = delete;
	RtvHeap(RtvHeap&&) = delete;
	RtvHeap(uint32_t heapSize);

	RtvHeap& operator=(const RtvHeap&) = delete;
	RtvHeap& operator=(RtvHeap&&) = delete;
public:
	~RtvHeap() = default;

public:
	static void Initialize(UINT heapSize);

	static void Finalize();

	static RtvHeap* const GetInstance();

private:
	static Lamb::SafePtr<RtvHeap> instance_;

private:
	void CreateDescriptorHeap(uint32_t heapSize) override;

	void CreateHeapHandles() override;

	[[deprecated("Don`t use this function")]]
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHeapHandle([[maybe_unused]]uint32_t heapIndex) override {
		return D3D12_GPU_DESCRIPTOR_HANDLE{};
	}

public:
	void CreateBackBuffer(
		std::array<Lamb::LambPtr<ID3D12Resource>, DirectXSwapChain::kBackBufferNumber_>& backBuffer,
		IDXGISwapChain4* const swapChain
		);
	
	void SetMainRtv();

public:
	void SetRtv(uint32_t heapHandle, uint32_t renderTargetNum = 1u, bool isDrawDepth = true);
	void SetRtv(std::initializer_list<D3D12_CPU_DESCRIPTOR_HANDLE> heapHandles, bool isDrawDepth = true);

	void ClearRenderTargetView(uint32_t handle, const class Vector4& clearColor);

	uint32_t CreateView(class RenderTarget& peraRender);
};