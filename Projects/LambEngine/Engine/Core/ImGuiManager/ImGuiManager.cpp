#include "ImGuiManager.h"

#include <cassert>

#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#include "Utils/SafeDelete/SafeDelete.h"

ImGuiManager* ImGuiManager::instance_ = nullptr;

ImGuiManager* const ImGuiManager::GetInstance() {
	return instance_;
}
void ImGuiManager::Initialize() {
	assert(!instance_);
	instance_ = new ImGuiManager{};
	assert(!!instance_);
}
void ImGuiManager::Finalize() {
	Lamb::SafeDelete(instance_);
}

ImGuiManager::ImGuiManager() {
#ifdef _DEBUG
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// SRV用のヒープ
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	uint32_t useHandle = descriptorHeap->BookingHeapPos(1u);

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// 日本語フォント追加
	ImGui::GetIO().Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WindowFactory::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(
		device,
		DirectXSwapChain::kBackBufferNumber_,
		rtvDesc.Format,
		descriptorHeap->Get(),
		descriptorHeap->GetCpuHeapHandle(useHandle),
		descriptorHeap->GetGpuHeapHandle(useHandle)
	);

	descriptorHeap->UseThisPosition(useHandle);
#endif // DEBUG
}
ImGuiManager::~ImGuiManager() {
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // DEBUG
}

void ImGuiManager::Start() {
#ifdef _DEBUG
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG
}

void ImGuiManager::End() {
#ifdef _DEBUG
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();
	// ImGui描画
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif // DEBUG
}