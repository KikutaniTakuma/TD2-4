#pragma once
#include <cstdint>
#include <array>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#undef max
#undef min
#include "Engine/EngineUtils/LambPtr/LambPtr.h"

class DirectXSwapChain {
private:
	DirectXSwapChain();
	DirectXSwapChain(const DirectXSwapChain&) = delete;
	DirectXSwapChain(DirectXSwapChain&&) = delete;

	DirectXSwapChain& operator=(const DirectXSwapChain&) = delete;
	DirectXSwapChain& operator=(DirectXSwapChain&&) = delete;
public:
	~DirectXSwapChain() = default;

public:
	static DirectXSwapChain* const GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static DirectXSwapChain* instance_;

public:
	/// <summary>
	/// ビューポートセット
	/// </summary>
	/// <param name="width">基本的にバックバッファと同じにする</param>
	/// <param name="height">基本的にバックバッファと同じにする</param>
	void SetViewPort(uint32_t width, uint32_t height);

	/// <summary>
	/// メインレンダーターゲットにセットする
	/// </summary>
	void SetMainRenderTarget();

	/// <summary>
	/// バックバッファのクリア
	/// </summary>
	void ClearBackBuffer();

	/// <summary>
	/// バックバッファの状態を変更する
	/// </summary>
	void ChangeBackBufferState();

	/// <summary>
	/// スワップチェインに画面の交換を行う
	/// </summary>
	void SwapChainPresent();

public:
	IDXGISwapChain4* const GetSwapChain() const {
		return swapChain_.Get();
	}

	inline D3D12_RESOURCE_DESC GetSwapchainBufferDesc() const {
		return swapChainResource_[0]->GetDesc();
	}

public:
	static constexpr uint32_t kBackBufferNumber_ = 2u;

private:
	Lamb::LambPtr<IDXGISwapChain4> swapChain_;
	std::array<Lamb::LambPtr<ID3D12Resource>, kBackBufferNumber_> swapChainResource_;
	bool isRenderState_;
};