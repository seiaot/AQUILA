#pragma once

#include <Windows.h>
#include <timeapi.h>

namespace Fwk
{
class Time
{
    Time(const Time&) = delete;
    void operator = (const Time&) = delete;

public:

    Time();
    ~Time();

    void Init(DWORD daltaMsMax);
    void Update();
    void Term();

    DWORD GetTime()const;
    float GetDeltaTime()const;

private:

    DWORD m_CurrentTime;
    DWORD m_PrevTime;
    DWORD m_DeltaTime;
    DWORD m_DeltaTimeMax;

};

}