#include "Fwk/Graphics/Font.h"

#include <DirectXMath.h>
#include <stdlib.h>

#include "Fwk/Framework.h"
#include "SpriteFont.h"

using namespace DirectX;

Font::Font()
	: m_Text()
	, m_Position(XMFLOAT2(0.0f, 0.0f))
	, m_Color(XMFLOAT4(1.0f,1.0f,1.0f,1.0f))
	, m_Rotation(0.0f)
	, m_Origin(XMFLOAT2(0.0f,0.0f))
	, m_Scale(XMFLOAT2(1.0f,1.0f))
	, m_Depth(0.5f)
	, m_pSpriteFont(nullptr)
{
	;
}

Font::~Font()
{
	Term();
}

void Font::Init(Fwk::SpriteFont* pSpriteFont)
{
	m_Text.clear();
	m_Position = XMFLOAT2( 0.0f,0.0f );
	m_Color = XMFLOAT4( 1.0f,1.0f,1.0f,1.0f );
	m_Rotation = 0.0f;
	m_Origin = XMFLOAT2(0.0f, 0.0f);
	m_Scale = XMFLOAT2(1.0f, 1.0f);
	m_Depth = 0.5f;
	SetPriority(0);

	m_pSpriteFont = pSpriteFont;
}

void Font::Term()
{
	m_pSpriteFont = nullptr;
	Clear();
}

void Font::Clear()
{
	m_Text.clear();
}

void Font::SetText(const wchar_t* pText)
{
	if (pText == nullptr)
		return;

	m_Text = pText;
}
void Font::SetText(const std::wstring& text)
{
	m_Text = text;
}

void Font::SetText(const char* pText)
{
	if (pText == nullptr)
		return;

	// 確保するワイド文字用バッファのサイズは、バイト数ではなく文字数を指定する。
	int charNum = (int)strlen(pText);
	wchar_t* wc = new wchar_t[charNum +1];

	if (wc == nullptr)
		return;

	memset(wc, 0, sizeof(wchar_t) * (charNum + 1));

	// 変換.
	MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED,
		pText,
		charNum,
		wc,
		charNum + 1
	);

	m_Text = wc;

	delete[] wc;
}
void Font::SetText(const std::string& text)
{
	SetText(text.c_str());
}

void Font::SetPosition(float x, float y)
{
	m_Position = XMFLOAT2(x,y);
}

void Font::SetColor(float r,float g, float b)
{
	m_Color = XMFLOAT4(r,g,b,1.0f);
}

void Font::SetOrigin(float x, float y)
{
	m_Origin = XMFLOAT2(x, y);
}

void Font::SetRotation(float rotDeg)
{
	m_Rotation = XMConvertToRadians(rotDeg);
}

void Font::SetScale(float scale_x, float scale_y)
{
	m_Scale = XMFLOAT2(scale_x, scale_y);
}

void Font::Draw()
{
	RenderManager_I->RequestRender(this);
}

void Font::OnDraw(IGraphics* g,Camera* pCamera)
{
	if (m_pSpriteFont == nullptr)
		return;

	if (m_Text.size() == 0)
		return;

	m_pSpriteFont->Draw(
		m_Text.c_str()
		, m_Position
		, m_Color
		, m_Rotation
		, m_Origin
		, m_Scale
		, m_Depth
	);
}

void Font::SetDepth(float d)
{
	m_Depth = d;
}

float Font::GetDepth()const
{
	return m_Depth;
}

RENDER_PASS Font::GetRenderPass()const
{
	return RENDER_PASS::SpriteFont;
}

RENDER_MODE Font::GetRenderMode()const
{
	return RENDER_MODE::Texture_Transparent;
}