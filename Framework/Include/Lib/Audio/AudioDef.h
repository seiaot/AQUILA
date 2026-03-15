#pragma once

// XAudio2関連
#pragma comment(lib, "xaudio2.lib")
#include<xaudio2.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace Lib {
namespace Audio {

struct WaveData
{
	WAVEFORMATEX format;
	char* buffer;
	size_t size;
};

}//namespace Audio
}//namespace Lib