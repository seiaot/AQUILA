#include "Fwk/Time/FPSMeasure.h"

#include <timeapi.h>
#pragma comment( lib, "winmm.lib" )

namespace Fwk
{

FPSMeasure::FPSMeasure()
	: m_FrameCount(0)
	, m_StartTime(0)
	, m_TargetFPS(0)
	, m_AveCalcCount(60)
{
	;
}

FPSMeasure::~FPSMeasure()
{
	;
}

void FPSMeasure::Init(uint32_t targetFPS,DWORD currentTime, Fwk::SpriteFont* pSpriteFont)
{
	//FPS計算周りの変数
	m_FrameCount = 0;
	m_StartTime = currentTime;
	m_TargetFPS = targetFPS;

	m_Font.Init(pSpriteFont);
	m_Font.SetScale(1.0f, 1.0f);
	m_Font.SetDepth(0.0f);
	m_Font.SetPosition(0.0f,0.0f);
	m_Font.SetColor(1.0f, 1.0f, 1.0f);
	m_Font.SetVisible(false);
}

void FPSMeasure::Term() {
	m_Font.Term();
}

void FPSMeasure::Update(DWORD currentTime)
{
	float AveFps = 0.0f;

	m_FrameCount %= m_AveCalcCount;

	//定期的に平均FPS計測＆計測開始時刻更新
	if (m_FrameCount == 0) {
		int elapsedTime = currentTime - m_StartTime;
		AveFps = (elapsedTime > 0) ? 1000.f / ((float)elapsedTime / (float)m_AveCalcCount) : 0.0f;
		m_StartTime = currentTime;

		if(m_Font.IsVisible())
		{
			wchar_t buffer[256] = { 0 };
			swprintf_s(buffer, 256, L"fps=%.1f delta=%.1f", AveFps, ((float)elapsedTime / (float)m_AveCalcCount));
			std::wstring text = buffer;
			m_Font.SetText(text);
		}
	}

	++m_FrameCount;
}

void FPSMeasure::Draw()
{
	m_Font.Draw();
}

int FPSMeasure::CalcWaitTime()const {
	DWORD elapsedTime = timeGetTime() - m_StartTime;
	DWORD targetElapsedTime = (DWORD)((1000.0f / m_TargetFPS) * m_FrameCount);
	int waitTime = (int)targetElapsedTime - (int)elapsedTime;
	return waitTime;
}

void FPSMeasure::SetVisible(bool isVisible) {
	m_Font.SetVisible(isVisible);
}

void FPSMeasure::SetDrawPosition(float x, float y) {
	m_Font.SetPosition(x, y);
}


}

