#pragma once

#include <mutex>
#include "AudioDef.h"

namespace Lib {
namespace Audio {

class SourceVoice : public IXAudio2VoiceCallback
{

public:

	SourceVoice();
	~SourceVoice();

	bool Init(IXAudio2* pXAudio,const WaveData* pWaveData);
	void Term();

	void Play(int PlayNum = 1);
	void Stop();
	void Pause();
	
	bool IsPlaying()const;
	bool IsPausing()const;

	void SetVolume(float volume);
	float GetVolume()const;

private:

	void OnVoiceProcessingPassStart(UINT32 BytesRequired) override;
	void OnVoiceProcessingPassEnd() override;
	void OnStreamEnd() override;
	void OnBufferStart(void* pBufferContext) override;
	void OnBufferEnd(void* pBufferContext) override;
	void OnLoopEnd (void* pBufferContext) override;
	void OnVoiceError(void* pBufferContext, HRESULT Error) override;

	void _stop();

private:

	IXAudio2SourceVoice* mp_SourceVoice;

	enum {
		DEFAULT_AUDIO_BUFFER_SIZE = 256
	};

	BYTE* mp_audioBuffer[2];
	UINT32 m_audioBufferSize[2];
	int m_bufferIndex;
	UINT32 m_audioReadBytes;

	const WaveData* mp_waveData;

	bool m_isPlaying;
	bool m_isPausing;

	std::mutex m_mutex;

	int m_PlayNum;
	int m_PlayCount;
};


}//namespace Audio
}//namespace Lib