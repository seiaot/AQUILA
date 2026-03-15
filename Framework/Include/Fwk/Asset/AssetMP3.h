// ----------------------------------------------------
// MP3アセットクラス
// 
// 
// 
// ----------------------------------------------------
#pragma once

#include "Asset.h"

#include "Lib/Audio/Audio.h"

namespace Fwk
{
class AssetMP3 : public Asset
{
	friend class AssetManager;
	friend class AssetHandle;

	AssetMP3();
	virtual ~AssetMP3();

public:

	const Lib::Audio::WaveData* GetWaveData()const;

protected:

	bool Init(const wchar_t* pFilePath) override;
	void OnTerm() override;


private:

	Lib::Audio::WaveData m_waveData;

	IMFSourceReader* m_pMFSourceReader;
	IMFMediaType* m_pMFMediaType;
};

}
