#include "Fwk/Time/Time.h"

#pragma comment( lib, "winmm.lib" )

namespace Fwk
{

Time::Time()
	: m_CurrentTime(0)
	, m_PrevTime(0)
	, m_DeltaTime(0)
	, m_DeltaTimeMax(0)
{
	;
}

Time::~Time()
{
	Term();
}

void Time::Init(DWORD daltaMsMax)
{
	m_CurrentTime = timeGetTime();
	m_PrevTime = m_CurrentTime;
	m_DeltaTimeMax = daltaMsMax;
}

void Time::Update()
{
	m_CurrentTime = timeGetTime();
	m_DeltaTime = m_CurrentTime - m_PrevTime;
	if (m_DeltaTime > m_DeltaTimeMax) {
		m_DeltaTime = m_DeltaTimeMax;
	}
	m_PrevTime = m_CurrentTime;
}

DWORD Time::GetTime()const
{
	return m_CurrentTime;
}

float Time::GetDeltaTime()const
{
	return (float)(m_DeltaTime*0.001f);
}

void Time::Term()
{
	;
}

}

