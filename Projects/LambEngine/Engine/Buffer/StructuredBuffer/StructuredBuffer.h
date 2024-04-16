#pragma once
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/Cocepts/Cocepts.h"
#include "Error/Error.h"
#include <cassert>
#include "Engine/Core/DescriptorHeap/Descriptor.h"

/// <summary>
/// ストラクチャードバッファ
/// </summary>
/// <typeparam name="T">ポインタと参照型以外をサポート</typeparam>
template<Lamb::IsNotReferenceAndPtr T, uint32_t bufferSize = 256u>
class StructuredBuffer final : public Descriptor {
public:
	StructuredBuffer() :
		bufferResource_(),
		srvDesc_(),
		data_(nullptr),
		isWright_(true),
		isCreateView_(false),
		range_(),
		roootParamater_()
	{
		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(T) * size());
#ifdef _DEBUG
		bufferResource_.SetName<decltype(*this)>();
#endif // _DEBUG
		srvDesc_ = {};
		srvDesc_.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc_.Buffer.FirstElement = 0;
		srvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc_.Buffer.NumElements = size();
		srvDesc_.Buffer.StructureByteStride = sizeof(T);

		if (isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		}

		range_ = {};
		range_.BaseShaderRegister = 1;
		range_.NumDescriptors = 1;
		range_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range_.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


		roootParamater_ = {};
		roootParamater_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		roootParamater_.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		roootParamater_.DescriptorTable.pDescriptorRanges = &range_;
		roootParamater_.DescriptorTable.NumDescriptorRanges = 1;
	}

	~StructuredBuffer() = default;

	inline StructuredBuffer(const StructuredBuffer& right) :
		StructuredBuffer{ right.Size() }
	{
		*this = right;
	}

	inline StructuredBuffer(StructuredBuffer&&) = delete;

	inline StructuredBuffer& operator=(const StructuredBuffer& right) {
		for (uint32_t i = 0; i < bufferSize; i++) {
			(*this)[i] = right[i];
		}

		return *this;
	}
	inline StructuredBuffer<T>& operator=(StructuredBuffer&&) = delete;

public:
	void OnWright() noexcept {
		if (!isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			isWright_ = !isWright_;
		}
	}

	void OffWright() noexcept {
		if (isWright_) {
			bufferResource_->Unmap(0, nullptr);
			isWright_ = !isWright_;
		}
	}

	template<Lamb::IsInt IsInt>
	T& operator[](IsInt index) {
		assert(static_cast<uint32_t>(index) < bufferSize);
		return data_[index];
	}

	template<Lamb::IsInt IsInt>
	const T& operator[](IsInt index) const {
		assert(static_cast<uint32_t>(index) < bufferSize);
		return data_[index];
	}

	constexpr uint32_t size() const {
		return bufferSize;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource_->GetGPUVirtualAddress();
	}

	const D3D12_ROOT_PARAMETER& GetRoootParamater() const noexcept {
		return roootParamater_;
	}

	void CreateView(
		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
		UINT heapHandle) noexcept
	{
		static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
		device->CreateShaderResourceView(bufferResource_.Get(), &srvDesc_, heapHandleCPU);
		heapHandleCPU_ = heapHandleCPU;
		heapHandleGPU_ = heapHandleGPU;
		heapHandle_ = heapHandle;

		isCreateView_ = true;
	}

private:
	Lamb::LambPtr<ID3D12Resource> bufferResource_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;

	D3D12_DESCRIPTOR_RANGE range_;
	D3D12_ROOT_PARAMETER roootParamater_;

	T* data_;

	bool isWright_;

	bool isCreateView_;
};