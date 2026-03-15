#pragma once

#include "AudioDef.h"

namespace Lib {
namespace Audio {

class SourceVoice;

class Audio {

public:

	Audio();
	~Audio();

	bool Init();
	void Term();

	void CreateSourceVoice(SourceVoice* pSource,const WaveData* pWaveData);

private:

	IXAudio2* mp_xaudio2;
	IXAudio2MasteringVoice* mp_masteringVoice;
};

}//namespace Audio
}//namespace Lib