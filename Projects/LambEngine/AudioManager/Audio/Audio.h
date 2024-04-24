#pragma once
#include <array>
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")


#include <string>
#include "Utils/SafePtr/SafePtr.h"

/// <summary>
/// 音の再生や停止、ロードを担う
/// </summary>
class Audio {
	friend class AudioManager;

private:
	struct ChunkHeader {
		std::array<char,4> id_;
		int32_t size_;
	};
	struct RiffHeader {
		ChunkHeader chunk_;
		std::array<char, 4> type_;
	};
	struct FormatChunk
	{
		ChunkHeader chunk_;
		WAVEFORMATEX fmt_;
	};

public:
	Audio();
	Audio(const Audio&) = delete;
	Audio(Audio&&) = delete;
	~Audio();

	Audio& operator=(const Audio&) = delete;
	Audio& operator=(Audio&&) = delete;

public:
	void Start(float volume, bool isLoop);

	void Pause();

	void ReStart();

	void Stop();

	bool IsStart() const {
		return isStart_;
	}

	void SetAudio(float volume);

	void Debug(const std::string& guiName);

private:
	void Unload();

	void Load(const std::string& fileName);

private:
	WAVEFORMATEX wfet_;
	Lamb::SafePtr<BYTE> pBuffer_;
	uint32_t bufferSize_;
	IXAudio2SourceVoice* pSourceVoice_;

	bool isLoop_;
	bool isStart_;
	float volume_;

	bool isLoad_;
	std::string fileName_;
};