#include "DepthBuffer.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Engine/Graphics/TextureManager/Texture/Texture.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include <cmath>
#include "Engine/Core/DescriptorHeap/DsvHeap.h"
#include "Error/Error.h"

DepthBuffer::DepthBuffer():
	tex_{},
	depthStencilResource_{},
	srvDesc_{},
	handle_{},
	hadleUINT_{}
{
	DirectXDevice* const directXDevice = DirectXDevice::GetInstance();

	// DepthStencilTextureをウィンドウサイズで作成
	depthStencilResource_ = directXDevice->CreateDepthStencilTextureResource(Lamb::ClientSize());
	assert(depthStencilResource_);
	if (!depthStencilResource_) {
		throw Lamb::Error::Code<DepthBuffer>("something error", "CreateDepthStencilTextureResource");
	}

	srvDesc_ = {};
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc_.Texture2D.MipLevels = 1;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	static DsvHeap* const dsvHeap = DsvHeap::GetInstance();
	dsvHeap->BookingHeapPos(1u);
	dsvHeap->CreateView(*this);
}

DepthBuffer::DepthBuffer(const Vector2& bufSize):
	tex_{},
	depthStencilResource_{},
	srvDesc_{},
	handle_{},
	hadleUINT_{}
{
	DirectXDevice* const directXDevice = DirectXDevice::GetInstance();

	// DepthStencilTextureをウィンドウサイズで作成
	depthStencilResource_ = directXDevice->CreateDepthStencilTextureResource(bufSize);
	assert(depthStencilResource_);
	if (!depthStencilResource_) {
		throw Lamb::Error::Code<DepthBuffer>("something error", "CreateDepthStencilTextureResource");
	}

	srvDesc_ = {};
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc_.Texture2D.MipLevels = 1;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	static DsvHeap* const dsvHeap = DsvHeap::GetInstance();
	dsvHeap->BookingHeapPos(1u);
	dsvHeap->CreateView(*this);
}

DepthBuffer::~DepthBuffer() {
	static DsvHeap* const dsvHeap = DsvHeap::GetInstance();
	dsvHeap->ReleaseView(hadleUINT_);

	if (depthStencilResource_) {
		depthStencilResource_.Reset();
	}
}

void DepthBuffer::CreateDepthView(D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32_t hadleUINT) {
	handle_ = handle;
	hadleUINT_ = hadleUINT;

	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, handle_);
}
void DepthBuffer::CreateView(
	D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU,
	D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
	UINT heapHandle
) {
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();

	device->CreateShaderResourceView(
		depthStencilResource_.Get(),
		&srvDesc_,
		heapHandleCPU
	);

	heapHandleCPU_ = heapHandleCPU;
	heapHandleGPU_ = heapHandleGPU;
	heapHandle_ = heapHandle;

	tex_.reset();
	tex_ = std::make_unique<Texture>();
	assert(tex_);
	tex_->Set(
		depthStencilResource_,
		srvDesc_,
		heapHandleGPU_,
		heapHandle_
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthBuffer::GetDepthHandle() const {
	return handle_;
}
Texture* const DepthBuffer::GetTex() const {
	return tex_.get();
}