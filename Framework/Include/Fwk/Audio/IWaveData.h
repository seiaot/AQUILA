#pragma once

#include "Lib/Audio/Audio.h"

class IWaveData
{
public:
    virtual const Lib::Audio::WaveData* GetWaveData()const = 0;
};