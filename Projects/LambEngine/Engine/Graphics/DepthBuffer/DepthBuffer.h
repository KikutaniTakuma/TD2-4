#pragma once
#include <memory>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#undef max
#undef min
#include "Engine/EngineUtils/LambPtr/LambPtr.h"
#include "Engine/Core/DescriptorHeap/Descriptor.h"

class DepthBuffer final : public Descriptor{
public:
	DepthBuffer();
	DepthBuffer(const class Vector2& bufSize);
	DepthBuffer(const DepthBuffer&) = delete;
	DepthBuffer(DepthBuffer&&) = delete;
	~DepthBuffer();

	DepthBuffer& operator=(const DepthBuffer&) = delete;
	DepthBuffer& operator=(DepthBuffer&&) = delete;

public:
	void CreateDepthView(D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32_t hadleUINT);
	void CreateView(D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
		UINT heapHandle
	) override;
	
public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthHandle() const;
	class Texture* const GetTex() const;

private:
	std::unique_ptr<class Texture> tex_;

	Lamb::LambPtr<ID3D12Resource> depthStencilResource_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;

	D3D12_CPU_DESCRIPTOR_HANDLE handle_;
	uint32_t hadleUINT_;
};