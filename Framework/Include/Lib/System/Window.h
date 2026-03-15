#pragma once

#include <Windows.h>
#include <stdint.h>
#include "Lib/Math/Vector.h"

namespace Lib {
namespace System {

class Window
{
	Window(const Window&) = delete;      // アクセス禁止.
	void operator = (const Window&) = delete;      // アクセス禁止.

public:

	Window();
	~Window();

	bool Init(
		uint32_t _w
		, uint32_t _h
		, const wchar_t* pClassName
		, const wchar_t* pWndTitle
		, WNDPROC WndProc
	);

	void Term();

	HINSTANCE GetInst()const;
	HWND GetHWND()const;

	Lib::Math::Vector2f TransformToClient(const Lib::Math::Vector2f& screenPos)const;

	void ToggleFullscreen();
	bool IsFullscreen()const;

private:

	//ウィンドウ関連
	HINSTANCE m_hInst;
	HWND	  m_hwnd;
	uint32_t m_Width;
	uint32_t m_Height;

	wchar_t m_ClassName[255];

	RECT m_windowRect;
	bool m_isFullscreen;

};

}
}