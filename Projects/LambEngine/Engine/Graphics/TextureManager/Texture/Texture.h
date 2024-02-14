#pragma once

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "../externals/DirectXTex/DirectXTex.h"
#include "../externals/DirectXTex/d3dx12.h"

#include <string>

#include "Math/Vector2.h"

#include "Engine/Core/DescriptorHeap/Descriptor.h"
#include "Engine/EngineUtils/LambPtr/LambPtr.h"

/// <summary>
/// テクスチャの読み込みやリソースの管理を担う
/// </summary>
class Texture final : public Descriptor {
	friend class TextureManager;

public:
	Texture();
	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&& tex) noexcept;

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& tex) noexcept;

public:
	inline explicit operator bool() const noexcept {
		return isLoad_;
	}

	inline bool operator!() const noexcept {
		return !isLoad_;
	}


/// <summary>
/// 画像読み込み用
/// </summary>
private:
	void Load(const std::string& filePath);
	void Load(const std::string& filePath, ID3D12GraphicsCommandList* commandList);

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metaData);
	[[nodiscard]]
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	[[nodiscard]]
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12GraphicsCommandList* commandList);

public:
	void Unload();

/// <summary>
/// View作成関数
/// </summary>
public:
	void CreateView(
		D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE descHeapHandleGPU, 
		UINT descHeapHandleUINT
	) override;

/// <summary>
/// CPU側のリソースを解放
/// </summary>
private:
	void ReleaseIntermediateResource();

public:
	bool CanUse() const {
		return static_cast<bool>(textureResouce_) && !static_cast<bool>(intermediateResource_) && isLoad_;
	}

public:
	void Use(UINT rootParamator);

	void Set(
		const Lamb::LambPtr<ID3D12Resource>& resource,
		D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc,
		D3D12_GPU_DESCRIPTOR_HANDLE handle,
		UINT handleUINT
		);


/// <summary>
/// メンバ変数
/// </summary>
private:
	Lamb::LambPtr<ID3D12Resource> textureResouce_;
	Lamb::LambPtr<ID3D12Resource> intermediateResource_;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;

	bool isLoad_;
	bool threadLoadFlg_;



	Vector2 size_;
public:
	inline const Vector2& getSize() const {
		return size_;
	}

private:
	std::string fileName_;
public:
	inline const std::string& GetFileName() const {
		return fileName_;
	}
};