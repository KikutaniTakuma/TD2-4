#include "DirectXDevice.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Math/Vector2.h"

#include <cassert>
#include <format>
#include <array>

#include "Error/Error.h"
#include "Utils/SafeDelete/SafeDelete.h"

DirectXDevice* DirectXDevice::instance_ = nullptr;

DirectXDevice* const DirectXDevice::GetInstance() {
	return instance_;
}

void DirectXDevice::Initialize() {
	instance_ = new DirectXDevice{};
}
void DirectXDevice::Finalize() {
	Lamb::SafeDelete(instance_);
}


DirectXDevice::DirectXDevice():
	incrementSRVCBVUAVHeap_(0u),
	incrementRTVHeap_(0u),
	incrementDSVHeap_(0u),
	incrementSAMPLER_(0u),
	device_{},
	dxgiFactory_{},
	useAdapter_{}
{
	// IDXGIFactory生成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXDevice>("somthing error", "CreateDXGIFactory");
	}

	// 使用するグラボの設定
	SettingAdapter();
	if (useAdapter_ == nullptr) {
		throw Lamb::Error::Code<DirectXDevice>("GPU not Found", "SettingAdapter");
	}

	// Deviceの初期化
	// 使用しているデバイスによってD3D_FEATURE_LEVELの対応バージョンが違うので成功するまでバージョンを変えて繰り返す
	CreateDevice();

#ifdef _DEBUG
	InfoQueue();
#endif

	CreateHeapIncrements();

	Lamb::AddLog("Initialize DirectXDevice succeeded");
}

void DirectXDevice::SettingAdapter() {
	useAdapter_ = nullptr;
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter_.GetAddressOf())) != DXGI_ERROR_NOT_FOUND;
		++i) {

		DXGI_ADAPTER_DESC3 adapterDesc{};
		HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);
		if (hr != S_OK) {
			throw Lamb::Error::Code<DirectXDevice>("GetDesc3() Failed", __func__);
		}

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Lamb::AddLog(ConvertString(std::format(L"Use Adapter : {}", adapterDesc.Description)));
			break;
		}
		useAdapter_.Reset();
	}
}

void DirectXDevice::CreateDevice() {
	// Deviceの初期化
	// 使用しているデバイスによってD3D_FEATURE_LEVELの対応バージョンが違うので成功するまでバージョンを変えて繰り返す
	std::array featureLevels = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
	};
	std::array featureLevelString = {
		"12.2", "12.1", "12.0"
	};

	for (size_t i = 0; i < featureLevels.size(); ++i) {
		HRESULT hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf()));

		if (SUCCEEDED(hr)) {
			Lamb::AddLog(std::format("FeatureLevel : {}", featureLevelString[i]));
			break;
		}
	}

	if (device_ == nullptr) {
		throw Lamb::Error::Code<DirectXDevice>("device not found", __func__);
	}
	Lamb::AddLog("Complete create D3D12Device");
}

#ifdef _DEBUG
void DirectXDevice::InfoQueue() const {
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラーの予期に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 解放
		infoQueue->Release();
	}
}
#endif // _DEBUG

void DirectXDevice::CreateHeapIncrements() {
	incrementSRVCBVUAVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	incrementRTVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	incrementDSVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	incrementSAMPLER_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}

[[nodiscard]]
ID3D12DescriptorHeap* DirectXDevice::CreateDescriptorHeap(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderrVisible
) {
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderrVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (!SUCCEEDED(device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap)))) {
		throw Lamb::Error::Code<DirectXDevice>("somthing error", __func__);
	}

	return descriptorHeap;
}

[[nodiscard]]
ID3D12Resource* DirectXDevice::CreateBufferResuorce(size_t sizeInBytes) {
	if (!device_) {
		OutputDebugStringA("device is nullptr!!");
		throw Lamb::Error::Code<DirectXDevice>("device is nullptr", __func__);
	}

	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProp{};
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定にする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;
	// バッファの場合はこれにする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	ID3D12Resource* resource = nullptr;
	resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(&uploadHeapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<DirectXDevice>("somthing error", __func__);
	}

	return resource;
}

[[nodiscard]]
ID3D12Resource* DirectXDevice::CreateDepthStencilTextureResource(int32_t width, int32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	ID3D12Resource* resource = nullptr;
	if (!SUCCEEDED(
		device_->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&resource))
	)) {
		throw Lamb::Error::Code<DirectXDevice>("somthing error", __func__);
	}

	return resource;
}

[[nodiscard]]
ID3D12Resource* DirectXDevice::CreateDepthStencilTextureResource(const Vector2& size) {
	return CreateDepthStencilTextureResource(
		static_cast<int32_t>(size.x),
		static_cast<int32_t>(size.y)
	);
}
