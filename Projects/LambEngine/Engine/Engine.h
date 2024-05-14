#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "EngineUtils/LambPtr/LambPtr.h"

#include <string>
#include <memory>
#include <unordered_map>

/// <summary>
/// 各種マネージャーやDirectX12関連、ウィンドウ関連の初期化と解放を担う
/// </summary>
class Engine {
private:
	Engine() = default;
	Engine(const Engine&) = delete;
	Engine(Engine&&) = default;
	~Engine() = default;

	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;

public:
	/// <summary>
	/// 失敗した場合内部でassertで止められる
	/// </summary>
	/// <param name="windowName">Windowの名前</param>
	/// <param name="windowSize">ウィンドウの大きさ(バックバッファの大きさも同じになる)</param>
	/// <param name="fpsLimit">最大fps設定デフォルトで60</param>
	/// <param name="isFullscreen">フルスクリーンモードか否か</param>
	static void Initialize(
		const std::string& windowName, 
		const class Vector2& windowSize, 
		float fpsLimit = 60.0f, 
		bool isFullscreen = false
	);

	static void Finalize();

	static bool IsFinalize();

private:
	/// <summary>
	/// シングルトンインスタンス
	/// </summary>
	static Engine* instance_;

private:
	bool isFinalize_ = false;

public:
	static inline Engine* const GetInstance() {
		return instance_;
	}

	static D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle();

private:
	std::string GetCpuName() const;

	void HardwareLog() const;

#ifdef _DEBUG
	///
	/// Debug用
	/// 
private:
	class Debug {
	public:
		Debug();
		~Debug();

	public:
		void InitializeDebugLayer();

	private:
		Lamb::LambPtr<ID3D12Debug1> debugController_;
	};

	static Debug debugLayer_;
#endif




	/// 
	/// DirctXDevice
	/// 
private:
	void InitializeDirectXDevice();

private:
	class DirectXDevice* directXDevice_ = nullptr;

	/// 
	/// DirectXCommand
	/// 
private:
	void InitializeDirectXCommand();
	void FinalizeDirectXCommand();

public:
	class DirectXCommand* const GetMainCommandlist() const {
		return directXCommand_;
	}

private:
	class DirectXCommand* directXCommand_ = nullptr;

/// 
/// DirectXCommand
/// 
private:
	void InitializeDirectXSwapChain();

private:
	class DirectXSwapChain* directXSwapChain_ = nullptr;

	
/// <summary>
/// DirectXTK
/// </summary>
private:
	void InitializeDirectXTK();

private:
	class StringOutPutManager* stringOutPutManager_ = nullptr;


	/// 
	/// 描画関係
	/// 
private:
	void InitializeDraw();

private:
	std::unique_ptr<class DepthBuffer> depthStencil_;

	///
	/// MainLoop
	/// 
public:
	static void FrameStart();

	static void FrameEnd();
};