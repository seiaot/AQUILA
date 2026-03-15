#pragma once

#include <string>
#include "Fwk/Asset/Asset.h"
#include "Lib/Audio/Audio.h"
#include "IWaveData.h"

class MP3Sound : public IWaveData
{
    MP3Sound(const MP3Sound&) = delete;
    void operator = (const MP3Sound&) = delete;

public:

    MP3Sound();
    virtual ~MP3Sound();

    virtual void Load(const char* filename);
    virtual void Load(const wchar_t* filename);
    virtual void Load(const std::string& filename);
    virtual void Load(const std::wstring& filename);

    virtual void Unload();

    const Lib::Audio::WaveData* GetWaveData()const override;

private:

    Fwk::AssetHandle m_mp3File;
};