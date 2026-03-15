#pragma once

#include "Lib/Audio/SourceVoice.h"
#include "IWaveData.h"

//音源クラス
//音源に再生音のデータを設定して再生する感じ

class WaveSound;

class SoundSource
{

public:

	SoundSource();
	~SoundSource();

	bool Init( const IWaveData& pIWaveData, unsigned short sourceNum = 1);
	void Term();

	void Play(int PlayNum = 1);
	void Stop();
	bool IsPlaying()const;

	void Pause();
	bool IsPausing()const;

	void SetVolume(float volume);
	float GetVolume()const;

private:

	Lib::Audio::SourceVoice* mp_sources;
	unsigned short m_sourseNum;
	unsigned short m_useSource;
	bool m_IsInitialized;
};
