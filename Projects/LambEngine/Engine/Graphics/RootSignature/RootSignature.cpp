#include "RootSignature.h"
#include <cassert>
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Engine.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Error/Error.h"

RootSignature::RootSignature():
	rootSignature_{},
	isTexture_(false),
	isOutRangeBorder_(false)
{
	rootParamater_ = {};
}
RootSignature::RootSignature(const RootSignature& right) {
	*this = right;
}
RootSignature::RootSignature(RootSignature&& right) noexcept {
	*this = std::move(right);
}
RootSignature& RootSignature::operator=(const RootSignature& right) {
	rootSignature_ = right.rootSignature_;
	rootParamater_ = right.rootParamater_;
	isTexture_ = right.isTexture_;

	return *this;
}
RootSignature& RootSignature::operator=(RootSignature&& right) noexcept {
	rootSignature_ = std::move(right.rootSignature_);
	rootParamater_ = std::move(right.rootParamater_);
	isTexture_ = std::move(right.isTexture_);

	return *this;
}

bool RootSignature::operator==(const RootSignature& right) const {
	if (rootParamater_.size() != right.rootParamater_.size()) {
		return false;
	}
	for (size_t i = 0; i < rootParamater_.size(); i++) {
		if (rootParamater_[i].first != right.rootParamater_[i].first) {
			return false;
		}
	}
	return  isTexture_ == right.isTexture_ && isOutRangeBorder_ == right.isOutRangeBorder_;
}
bool RootSignature::operator!=(const RootSignature& right) const {
	return !(*this == right);
}

void RootSignature::Create(D3D12_ROOT_PARAMETER* rootParamater, size_t rootParamaterSize, bool isTexture, bool isOutRangeBorder) {
	// RootSignatureの生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootParamater_.clear();
	rootParamater_.reserve(rootParamaterSize);
	for (size_t i = 0; i < rootParamaterSize; i++) {
		std::vector<D3D12_DESCRIPTOR_RANGE> ranges = {};
		if (rootParamater[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			for (uint32_t j = 0; j < rootParamater[i].DescriptorTable.NumDescriptorRanges;j++) {
				ranges.push_back(rootParamater[i].DescriptorTable.pDescriptorRanges[j]);
			}
		}
		rootParamater_.push_back({ rootParamater[i], ranges });
	}

	std::vector<D3D12_ROOT_PARAMETER> params = {};
	params.reserve(rootParamater_.size());
	for (auto& i : rootParamater_) {
		if (i.first.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			i.first.DescriptorTable.pDescriptorRanges = i.second.data();
			i.first.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(i.second.size());
		}
		params.push_back(i.first);
	}

	descriptionRootSignature.pParameters = params.data();
	descriptionRootSignature.NumParameters = static_cast<UINT>(params.size());

	isTexture_ = isTexture;
	isOutRangeBorder_ = isOutRangeBorder;

	D3D12_STATIC_SAMPLER_DESC staticSamplers{};
	if (isOutRangeBorder_) {
		staticSamplers.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		staticSamplers.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers.MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers.ShaderRegister = 0;
		staticSamplers.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		staticSamplers.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	}
	else {
		staticSamplers.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers.MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers.ShaderRegister = 0;
		staticSamplers.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}

	descriptionRootSignature.pStaticSamplers = isTexture_ ? &staticSamplers : nullptr;
	descriptionRootSignature.NumStaticSamplers = isTexture_ ? 1u : 0u;

	// シリアライズしてバイナリにする
	Lamb::LambPtr<ID3DBlob> signatureBlob;
	Lamb::LambPtr<ID3DBlob> errorBlob;
	HRESULT  hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		throw Lamb::Error::Code<RootSignature>(reinterpret_cast<char*>(errorBlob->GetBufferPointer()), ErrorPlace);
	}
	// バイナリをもとに生成
	if (rootSignature_) {
		rootSignature_.Reset();
	}
	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<RootSignature>("CreateRootSignature failed", ErrorPlace);
	}
}

bool operator==(const D3D12_ROOT_PARAMETER& left, const D3D12_ROOT_PARAMETER& right) {
	bool isSameDescriptorTable = false;
	if (left.ParameterType != right.ParameterType || 
		left.ShaderVisibility != right.ShaderVisibility) {
		return false;
	}
	else if(left.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE){
		if (left.DescriptorTable.NumDescriptorRanges == right.DescriptorTable.NumDescriptorRanges) {
			for (uint32_t i = 0; i < left.DescriptorTable.NumDescriptorRanges; i++) {
				isSameDescriptorTable =
					left.DescriptorTable.pDescriptorRanges[i].BaseShaderRegister == right.DescriptorTable.pDescriptorRanges[i].BaseShaderRegister
					&& left.DescriptorTable.pDescriptorRanges[i].NumDescriptors == right.DescriptorTable.pDescriptorRanges[i].NumDescriptors
					&& left.DescriptorTable.pDescriptorRanges[i].OffsetInDescriptorsFromTableStart == right.DescriptorTable.pDescriptorRanges[i].OffsetInDescriptorsFromTableStart
					&& left.DescriptorTable.pDescriptorRanges[i].RangeType == right.DescriptorTable.pDescriptorRanges[i].RangeType
					&& left.DescriptorTable.pDescriptorRanges[i].RegisterSpace == right.DescriptorTable.pDescriptorRanges[i].RegisterSpace;

				if (!isSameDescriptorTable) {
					return false;
				}
			}
		}
		else {
			return false;
		}
	}
	else if (left.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV) {
		if (left.Descriptor.RegisterSpace == right.Descriptor.RegisterSpace&&
			left.Descriptor.ShaderRegister == right.Descriptor.ShaderRegister
			) {

			return true;
		}
	}
	else {
		return false;
	}

	return true;
}

bool operator!=(const D3D12_ROOT_PARAMETER& left, const D3D12_ROOT_PARAMETER& right) {
	return !(left == right);
}

bool RootSignature::IsSame(D3D12_ROOT_PARAMETER* rootParamater, size_t rootParamaterSize, bool isTexture, bool isOutRangeBorder) const {
	if (rootParamater_.size() != rootParamaterSize) {
		return false;
	}
	for (size_t i = 0; i < rootParamater_.size();i++) {
		if (rootParamater_[i].first != rootParamater[i]) {
			return false;
		}
	}
	return isTexture_ == isTexture && isOutRangeBorder_ == isOutRangeBorder;
}