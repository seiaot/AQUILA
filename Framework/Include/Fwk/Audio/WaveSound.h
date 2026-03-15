#pragma once

#include <string>
#include "Fwk/Asset/Asset.h"
#include "Lib/Audio/Audio.h"
#include "IWaveData.h"

class WaveSound : public IWaveData
{
    WaveSound(const WaveSound&) = delete;
    void operator = (const WaveSound&) = delete;

public:

    WaveSound();
    virtual ~WaveSound();

    virtual void Load(const char* filename);
    virtual void Load(const wchar_t* filename);
    virtual void Load(const std::string& filename);
    virtual void Load(const std::wstring& filename);

    virtual void Unload();

    const Lib::Audio::WaveData* GetWaveData()const override;

private:

    Fwk::AssetHandle m_waveFile;
};