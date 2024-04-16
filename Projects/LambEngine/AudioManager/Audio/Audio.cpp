#include <fstream>
#include <filesystem>
#include "AudioManager/AudioManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "imgui.h"
#include "Error/Error.h"

Audio::Audio():
	wfet_(),
	pBuffer_(nullptr),
	bufferSize_(0u),
	pSourceVoice_(nullptr),
	isLoop_(false),
	isStart_(false),
	volume_(1.0f),
	isLoad_{false},
	fileName_{}
{}

Audio::~Audio() {
	delete[] pBuffer_;

	if (pMFMediaType_) {
		pMFMediaType_->Release();
	}
	if (pMFSourceReader_) {
		pMFSourceReader_->Release();
	}
	pBuffer_ = nullptr;
	bufferSize_ = 0;
	wfet_ = {};
}

void Audio::Load(const std::string& fileName) {
	if (!std::filesystem::exists(std::filesystem::path{ fileName })) {
		throw Lamb::Error::Code<Audio>(("This file is not found -> " + fileName), __func__);
	}
	if (std::filesystem::path(fileName).extension() == ".wav") {
		LoadWav(fileName);
	}
	else if (std::filesystem::path(fileName).extension() == ".mp3") {
		LoadMp3(fileName);
	}
	else {
		throw Lamb::Error::Code<Audio>(("This file is not supported (only ""mp3"" or ""wav"" file) -> " + fileName), __func__);
	}


	HRESULT hr = AudioManager::GetInstance()->xAudio2_->CreateSourceVoice(&pSourceVoice_, &wfet_);
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<Audio>("CreateSourceVoice() failed", "Load()");
	}

	isLoad_ = true;
}

void Audio::LoadWav(const std::string& fileName) {
	std::ifstream file;
	try {
		file.open(fileName, std::ios::binary);
	}
	catch (const std::exception& err) {
		throw Lamb::Error::Code<Audio>(err.what(), __func__);
	}

	fileName_ = fileName;

	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	if (strncmp(riff.chunk_.id_.data(), "RIFF", 4) != 0) {
		throw Lamb::Error::Code<Audio>("Not found RIFF", "Load()");
	}
	if (strncmp(riff.type_.data(), "WAVE", 4) != 0) {
		throw Lamb::Error::Code<Audio>("Not found WAVE", "Load()");
	}

	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	int32_t nowRead = 0;
	while (strncmp(format.chunk_.id_.data(), "fmt ", 4) != 0) {
		file.seekg(nowRead, std::ios_base::beg);
		if (file.eof()) {
			throw Lamb::Error::Code<Audio>("Not found fmt", "Load()");
		}
		nowRead++;
		file.read((char*)&format, sizeof(ChunkHeader));
	}

	if (format.chunk_.size_ > sizeof(format.fmt_)) {
		throw Lamb::Error::Code<Audio>("format chunk size is too big ->" + std::to_string(format.chunk_.size_) + " byte (max is " + std::to_string(sizeof(format.fmt_)) + " byte)", "Load()");
	}
	file.read((char*)&format.fmt_, format.chunk_.size_);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	if (strncmp(data.id_.data(), "JUNK", 4) == 0) {
		file.seekg(data.size_, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	while (strncmp(data.id_.data(), "data", 4) != 0) {
		file.seekg(data.size_, std::ios_base::cur);
		if (file.eof()) {
			throw Lamb::Error::Code<Audio>("Not found data", "Load()");
		}
		file.read((char*)&data, sizeof(data));
	}

	char* pBufferLocal = new char[data.size_];
	file.read(pBufferLocal, data.size_);

	file.close();

	wfet_ = format.fmt_;
	pBuffer_ = reinterpret_cast<BYTE*>(pBufferLocal);
	bufferSize_ = data.size_;
}

void Audio::LoadMp3(const std::string& fileName) {
	MFCreateSourceReaderFromURL(ConvertString(fileName).c_str(), NULL, &pMFSourceReader_);


	MFCreateMediaType(&pMFMediaType_);
	pMFMediaType_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pMFSourceReader_->SetCurrentMediaType(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM),
		nullptr, pMFMediaType_.get()
	);

	pMFMediaType_->Release();
	pMFMediaType_ = nullptr;
	pMFSourceReader_->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pMFMediaType_);

	Lamb::SafePtr<WAVEFORMATEX> pWaveFormat = &wfet_;
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType_.get(), &pWaveFormat, nullptr);
	wfet_ = *pWaveFormat;

	std::vector<BYTE> mediaData;
	while (true)
	{
		Lamb::SafePtr<IMFSample> pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		pMFSourceReader_->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}

		Lamb::SafePtr<IMFMediaBuffer> pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		Lamb::SafePtr<BYTE> localBuffer = nullptr;
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&localBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		std::memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, localBuffer.get(), cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	pBuffer_ = new BYTE[mediaData.size()];
	std::memcpy(pBuffer_, mediaData.data(), mediaData.size());

	bufferSize_ = static_cast<uint32_t>(mediaData.size() * sizeof(BYTE));
}


void Audio::Start(float volume, bool isLoop) {
	if (!isLoad_) {
		return;
	}

	HRESULT hr;
	volume_ = volume;

	Stop();
	if (!pSourceVoice_) {
		isLoop_ = isLoop;

		hr = AudioManager::GetInstance()->xAudio2_->CreateSourceVoice(&pSourceVoice_, &wfet_);
		XAUDIO2_BUFFER buf{};
		buf.pAudioData = pBuffer_;
		buf.AudioBytes = bufferSize_;
		buf.Flags = XAUDIO2_END_OF_STREAM;
		buf.LoopCount = isLoop_ ? XAUDIO2_LOOP_INFINITE : 0;

		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<Audio>("SubmitSourceBuffer() failed", __func__);
		}
		hr = pSourceVoice_->SubmitSourceBuffer(&buf);
	}
	hr = pSourceVoice_->Start();
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<Audio>("function is something error", __func__);
	}
	pSourceVoice_->SetVolume(volume_);

	isStart_ = true;
}

void Audio::Pause() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->Stop();
		isStart_ = false;
	}
}

void Audio::ReStart() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->Start();
		isStart_ = true;
	}
}

void Audio::Stop() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->DestroyVoice();
	}
	pSourceVoice_ = nullptr;

	isStart_ = false;
}

void Audio::SetAudio(float volume) {
	volume_ = volume;
	if (pSourceVoice_ && isStart_) {
		pSourceVoice_->SetVolume(volume_);
	}
}

void Audio::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat("volume", &volume_, 0.001f, 0.0f, 1.0f);
	SetAudio(volume_);
	
	ImGui::Checkbox("isLoop", &isLoop_);
	if (ImGui::Button("Start")) {
		Start(volume_, isLoop_);
	}
	if (ImGui::Button("Stop")) {
		Stop();
	}
	if (ImGui::Button("ReStart")) {
		ReStart();
	}
	if (ImGui::Button("Pause")) {
		Pause();
	}

	ImGui::End();
#endif // _DEBUG
}