#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

class Descriptor {
public:
	Descriptor() = default;
	virtual ~Descriptor() = default;

public:
	virtual void CreateView(
		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU, 
		D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU, 
		UINT heapHandle) = 0;

public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const {
		return heapHandleCPU_;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const {
		return heapHandleGPU_;
	}
	UINT GetHandleUINT() const {
		return heapHandle_;
	}


protected:
	D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU_;
	UINT heapHandle_;
};