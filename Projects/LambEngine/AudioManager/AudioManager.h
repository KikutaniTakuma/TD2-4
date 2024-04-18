#pragma once
#include <memory>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include "Audio/Audio.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Engine/EngineUtils/LambPtr/LambPtr.h"

/// <summary>
/// Audioを管理
/// </summary>
class AudioManager {
	friend Audio;

private:
	struct LoadStatus {
		std::string fileName_;
		Audio**const audio_;
	};

private:
	AudioManager();
	AudioManager(const AudioManager&) = delete;
	AudioManager(AudioManager&&) noexcept = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager& operator=(AudioManager&&) noexcept = delete;
public:
	~AudioManager();

public:
	static void Inititalize();
	static void Finalize();
	static inline AudioManager* const GetInstance() {
		return instance_.get();
	}

private:
	static Lamb::SafePtr<AudioManager> instance_;

public:
	Audio* const LoadWav(const std::string& fileName);
	void LoadWav(const std::string& fileName, Audio** const audio);

	void Unload(const std::string& fileName);

	void Unload(Audio* audio);

	IXAudio2MasteringVoice* GetMasterVoice() {
		return masterVoice_.get();
	}

public:
	void ThreadLoad();

	void CheckThreadLoadFinish();
private:
	void JoinThread();

private:
	Lamb::LambPtr<IXAudio2> xAudio2_;
	Lamb::SafePtr<IXAudio2MasteringVoice> masterVoice_;

	std::unordered_map<std::string, std::unique_ptr<Audio>> audios_;

	std::queue<LoadStatus> threadAudioBuff_;
	std::thread load_;
	std::mutex mtx_;
	bool isThreadLoadFinish_;
};