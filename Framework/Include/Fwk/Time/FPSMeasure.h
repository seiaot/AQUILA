#pragma once

#include <cstdint>

#include "Fwk/Graphics/Font.h"

namespace Fwk
{

class SpriteFont;

class FPSMeasure
{
    FPSMeasure(const FPSMeasure&) = delete;
    void operator = (const FPSMeasure&) = delete;

public:

    FPSMeasure();
    ~FPSMeasure();

    void Init(uint32_t targetFPS,DWORD currentTime,Fwk::SpriteFont* pSpriteFont);
    void Term();
    void Update(DWORD currentTime);
    void Draw();
    int CalcWaitTime()const;

    void SetVisible(bool isVisible);
    void SetDrawPosition(float x, float y);

private:

    Font m_Font;
    uint32_t m_FrameCount;
    DWORD m_StartTime;
    uint32_t m_TargetFPS;
    uint32_t m_AveCalcCount;
};

}