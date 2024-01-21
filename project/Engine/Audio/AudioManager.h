#pragma once
#include <xaudio2.h>
#include <fstream>
//#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
class AudioManager
{
public:
	struct ChunkHeader {
		char id[4];
		int32_t size;
	};
	struct RiffHeader {
		ChunkHeader chunk;
		char type[4];
	};
	struct formatChunk {
		ChunkHeader chunk;
		WAVEFORMATEX fmt;
	};
	struct SoundData {
		WAVEFORMATEX wfex;
		BYTE* pBuffer;
		uint32_t bufferSize_;
	};

	struct SoundDataContanor {
		SoundData data;
		std::string name;
	};
	static AudioManager* GetInstance();
	void Initialize(std::string directoryPath = "Resources/Audio/");
	void Finalize() {
		xaudio2_.Reset();
		UnLoadAll();
	};
	uint32_t LoadWave(const std::string& fileName);
	void SoundUnLoad(SoundData* soundData);
	void UnLoadAll();
	void PlayWave(uint32_t audioHandle);
private:
	AudioManager() = default;
	~AudioManager() = default;
	AudioManager(const AudioManager&) = delete;
	AudioManager operator=(const AudioManager&) = delete;
	Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;
	IXAudio2MasteringVoice* masterVoice_;
	std::string directoryPath_;
	uint32_t soundNum_;//SoundDataの数
	std::vector<std::unique_ptr<SoundDataContanor>> soundDatas_;
};
