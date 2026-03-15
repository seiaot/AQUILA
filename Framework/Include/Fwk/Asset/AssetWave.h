// ----------------------------------------------------
// Waveアセットクラス
// 
// 
// 
// ----------------------------------------------------
#pragma once

#include "Asset.h"

#include "Lib/Audio/Audio.h"

namespace Fwk
{
class AssetWave : public Asset
{
	friend class AssetManager;
	friend class AssetHandle;

	AssetWave();
	virtual ~AssetWave();

public:

	const Lib::Audio::WaveData* GetWaveData()const;


protected:

	bool Init(const wchar_t* pFilePath) override;
	void OnTerm() override;


private:

	Lib::Audio::WaveData m_waveData;

};

}
