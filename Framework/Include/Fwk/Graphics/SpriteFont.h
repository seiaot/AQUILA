#pragma once

#include <d3d12.h>

//dxtk -------------------------------
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>
//------------------------------------

namespace Lib {
namespace Graphics {
class IGraphics;
class DescriptorHeap;
class DescriptorHandle;
}
}

namespace Fwk {

class SpriteFont
{
    SpriteFont(const SpriteFont&) = delete;
    void operator = (const SpriteFont&) = delete;

public:

    SpriteFont();
    ~SpriteFont();

    bool Init(
        Lib::Graphics::IGraphics* pGraphics,
        const wchar_t* pFontFileName
    );
    void Term();

    void BeginRender(Lib::Graphics::IGraphics* pGraphics);
    void EndRender();

    void PostPresent(Lib::Graphics::IGraphics* pGraphics);

    void Draw(
        const char* pText, 
        const DirectX::XMFLOAT2& location,
        const DirectX::XMFLOAT4& color,
        float rotation,
        const DirectX::XMFLOAT2& origin,
        const DirectX::XMFLOAT2& scale,
        float depth
    );

    void Draw(
        const wchar_t* pText,
        const DirectX::XMFLOAT2& location,
        const DirectX::XMFLOAT4& color,
        float rotation,
        const DirectX::XMFLOAT2& origin,
        const DirectX::XMFLOAT2& scale,
        float depth
    );

private:

    DirectX::GraphicsMemory* m_pGraphicsMemory;
    DirectX::SpriteFont* m_pSpriteFont;
    DirectX::SpriteBatch* m_pSpriteBatch;

    Lib::Graphics::DescriptorHeap* m_pHeapSRV;
    Lib::Graphics::DescriptorHandle* m_pHeapHandle;

};

}//namespace Fwk