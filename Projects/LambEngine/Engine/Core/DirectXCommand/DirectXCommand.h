#pragma once

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#undef max
#undef min

#include "Engine/EngineUtils/LambPtr/LambPtr.h"

class DirectXCommand {
public:
	DirectXCommand();
	DirectXCommand(const DirectXCommand&) = delete;
	DirectXCommand(DirectXCommand&&) = delete;

	DirectXCommand& operator=(const DirectXCommand&) = delete;
	DirectXCommand& operator=(DirectXCommand&&) = delete;
public:
	~DirectXCommand();

public:
	static DirectXCommand* const GetMainCommandlist();


public:
	/// <summary>
	/// コマンドリストを確定させる
	/// </summary>
	void CloseCommandlist();

	/// <summary>
	/// コマンドリストを実行
	/// </summary>
	void ExecuteCommandLists();

	/// <summary>
	/// コマンドリストのリセット(また使えるようにする)
	/// </summary>
	void ResetCommandlist();

	/// <summary>
	/// コマンドリストの実行を待つ
	/// </summary>
	void WaitForFinishCommnadlist();

private:
	void CreateCommandQueue();

	void CreateCommandAllocator();

	void CreateGraphicsCommandList();

	void CrateFence();

public:
	ID3D12GraphicsCommandList* const GetCommandList() const {
		return commandList_.Get();
	}

	ID3D12CommandQueue* const GetCommandQueue() const {
		return commandQueue_.Get();
	}

	inline bool GetIsCloseCommandList() const {
		return isCommandListClose_;
	}

private:
	Lamb::LambPtr<ID3D12CommandQueue> commandQueue_;
	Lamb::LambPtr<ID3D12CommandAllocator> commandAllocator_;
	Lamb::LambPtr<ID3D12GraphicsCommandList> commandList_;
	bool isCommandListClose_;

	Lamb::LambPtr<ID3D12Fence> fence_;
	uint64_t fenceVal_;
	HANDLE fenceEvent_;
};
/// <summary>
/// バリア
/// </summary>
/// <param name="resource">リソースバリアを貼るリソース</param>
/// <param name="before">今の状態</param>
/// <param name="after">遷移後の状態</param>
/// <param name="subResource">サブリソース</param>
void Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subResource = 0u);