#include "Lib/Audio/Audio.h"
#include "Lib/Util/Log.h"
#include "Lib/Audio/SourceVoice.h"

namespace Lib {
namespace Audio {


Audio::Audio()
	: mp_xaudio2(nullptr)
	, mp_masteringVoice(nullptr)
{

}

Audio::~Audio(){

}

bool Audio::Init() {

	HRESULT result;

	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

	//XAUDIO2生成
	result = XAudio2Create(&mp_xaudio2);
	if (FAILED(result)){
		return false;
	}

	//マスタリングボイス生成
	result = mp_xaudio2->CreateMasteringVoice(&mp_masteringVoice);
	if (FAILED(result)){
		return 0;
	}

	return true;

}
void Audio::Term() {

	if (mp_masteringVoice) {
		mp_masteringVoice->DestroyVoice();
		mp_masteringVoice = nullptr;;
	}

	if (mp_xaudio2) {
		mp_xaudio2->Release();
		mp_xaudio2 = nullptr;
	}

	MFShutdown();
}
void Audio::CreateSourceVoice(SourceVoice* pSource,const WaveData* pWaveData) {
	if (mp_xaudio2 == nullptr) {
		return;
	}
	if (pSource == nullptr) {
		return;
	}
	if (pWaveData == nullptr) {
		return;
	}
	pSource->Init(mp_xaudio2, pWaveData);
}

}
}