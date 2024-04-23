#include "AudioManager.h"
#include "../ConvertString.h"
#include <cassert>

#pragma comment(lib,"Mf.lib")
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")

AudioManager* AudioManager::GetInstance()
{
	static AudioManager instance;
	return &instance;
}

void AudioManager::Initialize(std::string directoryPath) {

	directoryPath_ = directoryPath;
	HRESULT hr = XAudio2Create(&xaudio2_,0,XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xaudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
	soundNum_ = 0;
	soundDatas_.clear();
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

void AudioManager::Finalize() {
	xaudio2_.Reset();
	UnLoadAll();
	MFShutdown();
};

uint32_t AudioManager::LoadWave(const std::string& fileName) {
	
	//読み込み済Texture
	uint32_t handle = soundNum_;
	// 読み込み済みテクスチャを検索
	auto it = std::find_if(soundDatas_.begin(), soundDatas_.end(), [&](const auto& soundData) {
		return soundData->name == fileName;
		});
	if (it != soundDatas_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(soundDatas_.begin(), it));
		return handle;
	}
	//handle++;

	std::ifstream file;
	std::string filePath = directoryPath_ + fileName;
	file.open(filePath,std::ios_base::binary);
	assert(file.is_open());
	RiffHeader riff;
	file.read((char*)&riff,sizeof(riff));
	if (strncmp(riff.chunk.id,"RIFF",4) != 0) {
		assert(0);
	}
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//フォーマット読み込み
	formatChunk format = {};
	file.read((char*)&format,sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt,format.chunk.size);
	//ジャンクチャンク読み飛ばし
	ChunkHeader data;
	file.read((char*)&data,sizeof(data));
	if (strncmp(data.id ,"JUNK",4) == 0) {
		file.seekg(data.size,std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}
	//データ読み込み
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	char* pBuffer = new char[data.size];
	file.read(pBuffer,data.size);
	//データをコンテナに積み込み
	std::unique_ptr<SoundDataContanor> soundData;
	soundData.reset(new SoundDataContanor);
	soundData->data.wfex = format.fmt;
	soundData->data.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData->data.bufferSize_ = data.size;
	soundData->name = fileName;
	soundDatas_.push_back(std::move(soundData));
	return ++handle - 1;
}

uint32_t AudioManager::LoadAudio(const std::string& fileName) {

	//読み込み済
	uint32_t handle = soundNum_;
	// 読み込み済みを検索
	auto it = std::find_if(soundDatas_.begin(), soundDatas_.end(), [&](const auto& soundData) {
		return soundData->name == fileName;
		});
	if (it != soundDatas_.end()) {
		// 読み込み済みの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(soundDatas_.begin(), it));
		return handle;
	}

	IMFSourceReader* pMFSourceReader{nullptr};
	IMFMediaType* pMFMediaType{nullptr};
	std::string filePath = directoryPath_ + fileName;
	//microsoftmediafoundation
	//CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	MFCreateSourceReaderFromURL(ConvertString(filePath).c_str(), NULL, &pMFSourceReader);

	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);
	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

	WAVEFORMATEX* waveFormat{nullptr};
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType,&waveFormat,nullptr);

	std::vector<BYTE> mediaData;
	while (1) {
		IMFSample* pMFSample{nullptr};
		DWORD dwStreamFlags{0};
		pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM,0,nullptr,&dwStreamFlags,nullptr,&pMFSample);
	
		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}
	
		IMFMediaBuffer* pMFMediaBuffer{nullptr};
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer{nullptr};
		DWORD cbCurrentLength{0};
		pMFMediaBuffer->Lock(&pBuffer,nullptr,&cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data()+ mediaData.size() - cbCurrentLength,pBuffer,cbCurrentLength);

		pMFMediaBuffer->Unlock();
		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	BYTE* pBuffer = new BYTE[mediaData.size()];
	memcpy(pBuffer, mediaData.data(), mediaData.size());

	//データをコンテナに積み込み
	std::unique_ptr<SoundDataContanor> soundData;
	soundData.reset(new SoundDataContanor);
	soundData->data.wfex = *waveFormat;
	soundData->data.pBuffer = pBuffer;
	soundData->data.bufferSize_ =static_cast<UINT32>(mediaData.size());
	soundData->name = fileName;

	//CoTaskMemFree(waveFormat);
	pMFMediaType->Release();
	pMFSourceReader->Release();

	//CoUninitialize();

	soundDatas_.push_back(std::move(soundData));
	return ++handle - 1;
}

void AudioManager::SoundUnLoad(SoundData* soundData) {
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize_ = 0;
	soundData->wfex = {};
}

void AudioManager::UnLoadAll() {
	for (std::vector<std::unique_ptr<SoundDataContanor>>::iterator ite = soundDatas_.begin(); ite != soundDatas_.end();ite++) {
		SoundUnLoad(&(*ite)->data);
		ite->reset();
	}
	soundDatas_.clear();
	soundNum_ = 0;
}

void AudioManager::PlayWave(uint32_t audioHandle) {
	HRESULT hr;
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xaudio2_->CreateSourceVoice(&pSourceVoice, &(soundDatas_[audioHandle]->data.wfex));
	assert(SUCCEEDED(hr));

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[audioHandle]->data.pBuffer;
	buf.AudioBytes = soundDatas_[audioHandle]->data.bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	hr = pSourceVoice->Start();
}