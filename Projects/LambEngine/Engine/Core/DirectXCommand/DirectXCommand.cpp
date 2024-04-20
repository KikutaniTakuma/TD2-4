#include "DirectXCommand.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include <cassert>
#include "Error/Error.h"
#include "Engine/Engine.h"
#include "Utils/SafePtr/SafePtr.h"


DirectXCommand::DirectXCommand():
	commandQueue_{},
	commandAllocator_{},
	commandList_{},
	isCommandListClose_{false},
	fence_{},
	fenceVal_{0llu},
	fenceEvent_{nullptr}
{
	CreateCommandQueue();

	CreateCommandAllocator();

	CreateGraphicsCommandList();

	CrateFence();

	Lamb::AddLog("Initialize DirectXCommand succeeded");
}

DirectXCommand::~DirectXCommand() {
	CloseHandle(fenceEvent_);
}

DirectXCommand* const DirectXCommand::GetMainCommandlist()
{
	return Engine::GetInstance()->GetMainCommandlist();
}

void DirectXCommand::CloseCommandlist() {
	// コマンドリストを確定させる
	HRESULT hr = commandList_->Close();
	isCommandListClose_ = true;
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXCommand>("commandList_->Close() failed", __func__);
	}
}

void DirectXCommand::ExecuteCommandLists() {
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void DirectXCommand::ResetCommandlist() {
	// 次フレーム用のコマンドリストを準備
	HRESULT hr = commandAllocator_->Reset();
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXCommand>("commandAllocator_->Reset() faield", __func__);
	}
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXCommand>("commandList_->Reset() faield", __func__);
	}
	isCommandListClose_ = false;
}

void DirectXCommand::WaitForFinishCommnadlist() {
	// Fenceの値を更新
	fenceVal_++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceVal_);

	// Fenceの値が指定したSigna値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceVal_) {
		// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
		// イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

void DirectXCommand::CreateCommandQueue() {
	Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();
	
	// コマンドキューを作成
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXCommand>("device somethig error", __func__);
	}
	else {
		Lamb::AddLog(std::string{__func__} + " succeeded");
	}

	commandQueue_.SetName<DirectXCommand>();
}

void DirectXCommand::CreateCommandAllocator() {
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();

	// コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXCommand>("device somethig error", __func__);
	}
	else {
		Lamb::AddLog(std::string{ __func__ } + " succeeded");
	}
	commandAllocator_.SetName<DirectXCommand>();
}

void DirectXCommand::CreateGraphicsCommandList() {
	Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();
	
	// コマンドリストを作成する
	commandList_ = nullptr;
	HRESULT hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
	
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXCommand>("device somethig error", __func__);
	}
	else {
		Lamb::AddLog(std::string{ __func__ } + " succeeded");
	}
	commandList_.SetName<DirectXCommand>();
}

void DirectXCommand::CrateFence() {
	Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();
	
	// 初期値0でFenceを作る
	fence_ = nullptr;
	fenceVal_ = 0llu;
	HRESULT hr = device->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
	fence_.SetName<DirectXCommand>();
	
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXCommand>("device somethig error", __func__);
	}

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = nullptr;
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!(fenceEvent_ != nullptr)) {
		throw Lamb::Error::Code<DirectXCommand>("device somethig error", __func__);
	}

	Lamb::AddLog(std::string{ __func__ } + " succeeded");
}

void Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subResource) {
	Lamb::SafePtr commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース
	barrier.Transition.pResource = resource;
	// subResourceの設定
	barrier.Transition.Subresource = subResource;
	// 遷移前(現在)のResouceState
	barrier.Transition.StateBefore = before;
	// 遷移後のResouceState
	barrier.Transition.StateAfter = after;
	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);
}