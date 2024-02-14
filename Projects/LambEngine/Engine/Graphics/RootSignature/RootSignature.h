#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <vector>

#include "Engine/EngineUtils/LambPtr/LambPtr.h"

/// <summary>
/// ルートシグネチャを管理
/// </summary>
class RootSignature {
public:
	RootSignature();
	~RootSignature() = default;

	RootSignature(const RootSignature& right);
	RootSignature(RootSignature&& right) noexcept;

public:
	RootSignature& operator=(const RootSignature& right);
	RootSignature& operator=(RootSignature&& right) noexcept;

	bool operator==(const RootSignature& right) const;
	bool operator!=(const RootSignature& right) const;

public:
	void Create(D3D12_ROOT_PARAMETER* rootParamater, size_t rootParamaterSize, bool isTexture, bool isOutRangeBorder);

	inline ID3D12RootSignature* Get() const {
		return rootSignature_.Get();
	}

	bool IsSame(D3D12_ROOT_PARAMETER* rootParamater, size_t rootParamaterSize, bool isTexture, bool isOutRangeBorder) const;

private:
	Lamb::LambPtr<ID3D12RootSignature> rootSignature_;
	std::vector<std::pair<D3D12_ROOT_PARAMETER, std::vector<D3D12_DESCRIPTOR_RANGE>>> rootParamater_;
	
	bool isTexture_;
	bool isOutRangeBorder_;
};

bool operator==(const D3D12_ROOT_PARAMETER& left, const D3D12_ROOT_PARAMETER& right);
bool operator!=(const D3D12_ROOT_PARAMETER& left, const D3D12_ROOT_PARAMETER& right);