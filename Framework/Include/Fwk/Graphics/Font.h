#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <string>

#include "RenderObject.h"

using namespace DirectX;

namespace Fwk
{
class SpriteFont;
}

class Font : public Fwk::RenderObject
{
    Font(const Font&) = delete;
    void operator = (const Font&) = delete;

public:

    Font();
    ~Font();

    void Init(Fwk::SpriteFont* pSpriteFont);
    void Term();
    void Clear();

    void SetText(const wchar_t* pText);
    void SetText(const std::wstring& text);

    void SetText(const char* pText);
    void SetText(const std::string& text);

    void SetPosition(float x,float y);
    void SetColor(float r,float g,float b);
    void SetRotation(float rotDeg);
    void SetOrigin(float x, float y);
    void SetScale(float scale_x, float scale_y);

    void Draw();

    void OnDraw(IGraphics* g,Camera* pCamera) override;
    RENDER_PASS GetRenderPass()const override;
    RENDER_MODE GetRenderMode()const override;

    void SetSpriteFont(Fwk::SpriteFont* pSpriteFont);

    void SetDepth(float d);
    float GetDepth()const;

private:

    std::wstring m_Text;
    XMFLOAT2 m_Position;
    XMFLOAT4 m_Color;
    float m_Rotation;
    XMFLOAT2 m_Origin;
    XMFLOAT2 m_Scale;

    float m_Depth;

    Fwk::SpriteFont* m_pSpriteFont;
};