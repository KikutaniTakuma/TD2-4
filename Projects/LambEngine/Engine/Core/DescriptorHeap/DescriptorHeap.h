#pragma once

#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXSwapChain/DirectXSwapChain.h"
#include <list>
#include <deque>
#include <vector>

class DescriptorHeap {
protected:
	DescriptorHeap();
	DescriptorHeap(const DescriptorHeap&) = delete;
	DescriptorHeap(DescriptorHeap&&) = delete;

	DescriptorHeap& operator=(const DescriptorHeap&) = delete;
	DescriptorHeap& operator=(DescriptorHeap&&) = delete;
public:
	virtual ~DescriptorHeap() = default;

protected:
	/// <summary>
	/// ディスクリプタヒープ生成関数
	/// </summary>
	virtual void CreateDescriptorHeap(uint32_t heapSize) = 0;

public:
	/// <summary>
	/// CPUハンドルを取得
	/// </summary>
	/// <param name="heapIndex">取得したいハンドルのインデックス</param>
	/// <returns>CPUハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHeapHandle(uint32_t heapIndex) {
		return heapHandles_[heapIndex].first;
	}
	/// <summary>
	/// GPUハンドルを取得
	/// </summary>
	/// <param name="heapIndex">取得したいハンドルのインデックス</param>
	/// <returns>GPUハンドル</returns>
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHeapHandle(uint32_t heapIndex) {
		return heapHandles_[heapIndex].second;
	}

	/// <summary>
	/// ヒープの大きさ
	/// </summary>
	/// <returns>ヒープの大きさ</returns>
	inline UINT GetSize() const {
		return heapSize_;
	}

	/// <summary>
	/// ディスクリプタヒープのポインタ
	/// </summary>
	/// <returns>ヒープのポインタ</returns>
	inline ID3D12DescriptorHeap* const Get() const {
		return heap_.Get();
	}

public:
	/// <summary>
	/// ディスクリプタを作る場所の予約関数
	/// </summary>
	/// <param name="nextCreateViewNum">予約するディスクリプタの数</param>
	/// <returns>予約したヒープハンドルの一番最初のハンドル</returns>
	uint32_t BookingHeapPos(UINT nextCreateViewNum);

	/// <summary>
	/// ディスクリプタの解放関数
	/// </summary>
	/// <param name="viewHandle"></param>
	virtual void ReleaseView(UINT viewHandle);

	/// <summary>
	/// Useハンドルコンテナに追加(既に追加済みなら追加されない)
	/// </summary>
	/// <param name="handle">解放するハンドル</param>
	void UseThisPosition(uint32_t handle);

protected:
	/// <summary>
	/// ヒープのハンドルを作る
	/// </summary>
	virtual void CreateHeapHandles();

protected:
	Lamb::LambPtr<ID3D12DescriptorHeap> heap_;

	UINT heapSize_;
	UINT currentHandleIndex_;

	std::list<uint32_t> releaseHandle_;
	std::list<uint32_t> useHandle_;
	std::deque<uint32_t> bookingHandle_;


	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> heapHandles_;
};