#pragma once
#include "DescriptorHeap.h"
#include "Engine/Graphics/TextureManager/Texture/Texture.h"
#include "Engine/Graphics/RenderTarget/RenderTarget.h"
#include "Utils/ExecutionLog/ExecutionLog.h"

#include "Error/Error.h"

/// <summary>
/// ディスクリプタヒープ管理クラス
/// </summary>
class CbvSrvUavHeap final : public DescriptorHeap {
private:
	CbvSrvUavHeap() = delete;
	CbvSrvUavHeap(const CbvSrvUavHeap& right) = delete;
	CbvSrvUavHeap(CbvSrvUavHeap&& right) noexcept = delete;
	CbvSrvUavHeap(UINT numDescriptor);
	CbvSrvUavHeap& operator=(const CbvSrvUavHeap& right) = delete;
	CbvSrvUavHeap& operator=(CbvSrvUavHeap&& right) noexcept = delete;
public:
	~CbvSrvUavHeap() = default;

public:
	static void Initialize(UINT heapSize);

	static void Finalize();

	static CbvSrvUavHeap* const GetInstance();

private:
	static CbvSrvUavHeap* instance_;

private:
	void CreateDescriptorHeap(uint32_t heapSize) override;

public:
	void SetHeap();
	void Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex);
	void Use(uint32_t handleIndex, UINT rootParmIndex);

public:
	/// <summary>
	/// View生成用関数
	/// </summary>
	/// <param name="buffer"></param>
	/// <returns></returns>
	uint32_t CreateView(class Descriptor& buffer);
};