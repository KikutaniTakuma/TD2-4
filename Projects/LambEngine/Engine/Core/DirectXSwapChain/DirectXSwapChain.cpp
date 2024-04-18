#include "DirectXSwapChain.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DescriptorHeap/RtvHeap.h"
#include "Engine/Engine.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include <cassert>

#include "Error/Error.h"
#include "Utils/SafeDelete/SafeDelete.h"

Lamb::SafePtr<DirectXSwapChain> DirectXSwapChain::instance_ = nullptr;

DirectXSwapChain* const DirectXSwapChain::GetInstance() {
	return instance_.get();
}

void DirectXSwapChain::Initialize() {
	instance_.reset(new DirectXSwapChain());
}
void DirectXSwapChain::Finalize() {
	instance_.reset();
}

DirectXSwapChain::DirectXSwapChain():
	swapChain_{},
	swapChainResource_{},
	isRenderState_{false}
{
	IDXGIFactory7* const dxgiFactory = DirectXDevice::GetInstance()->GetDxgiFactory();
	ID3D12CommandQueue* const commandQueue = DirectXCommand::GetMainCommandlist()->GetCommandQueue();

	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();

	// スワップチェーンの作成
	swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = static_cast<UINT>(clientSize.x);
	swapChainDesc.Height = static_cast<UINT>(clientSize.y);
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = kBackBufferNumber_;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, WindowFactory::GetInstance()->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXSwapChain>("something error", "CreateSwapChainForHwnd");
	}

	swapChain_.SetName<DirectXSwapChain>();

	dxgiFactory->MakeWindowAssociation(
		WindowFactory::GetInstance()->GetHwnd(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	rtvHeap->CreateBackBuffer(swapChainResource_, swapChain_.Get());

	for (auto& i : swapChainResource_) {
		i.SetName<DirectXSwapChain>();
	}

	Lamb::AddLog("Initialize DirectXSwapChain succeeded");
}

void DirectXSwapChain::SetViewPort(uint32_t width, uint32_t height) {
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();

	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようになる
	scissorRect.left = 0;
	scissorRect.right = static_cast<LONG>(WindowFactory::GetInstance()->GetWindowSize().x);
	scissorRect.top = 0;
	scissorRect.bottom = static_cast<LONG>(WindowFactory::GetInstance()->GetWindowSize().y);
	commandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXSwapChain::SetMainRenderTarget() {
	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	rtvHeap->SetMainRtv();
}

void DirectXSwapChain::ClearBackBuffer() {
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();


	auto dsvH = Engine::GetDsvHandle();
	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// 指定した色で画面全体をクリアする
	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	Vector4 clearColor = { 0.1f, 0.25f, 0.5f, 0.0f };
	rtvHeap->ClearRenderTargetView(backBufferIndex, clearColor);
}

void DirectXSwapChain::ChangeBackBufferState() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	Barrier(
		swapChainResource_[backBufferIndex].Get(),
		isRenderState_ ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_PRESENT,
		isRenderState_ ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	isRenderState_ = !isRenderState_;
}

void DirectXSwapChain::SwapChainPresent() {
	// GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);
}