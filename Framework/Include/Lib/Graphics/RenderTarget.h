#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <cstdint>

#include "ComPtr.h"

namespace Lib {
namespace Graphics {

//前方宣言
class DescriptorHandle;
class DescriptorHeap;

class RenderTarget
{
    RenderTarget(const RenderTarget&) = delete;
    void operator = (const RenderTarget&) = delete;

public:
    RenderTarget();
    ~RenderTarget();

    bool Init(
        ID3D12Device* pDevice,
        DescriptorHeap* pHeapRTV,
        uint32_t        width,
        uint32_t        height,
        DXGI_FORMAT     format
    );

    bool InitFromBackBuffer(
        ID3D12Device* pDevice,
        DescriptorHeap* pHeapRTV,
        uint32_t        index,
        IDXGISwapChain* pSwapChain
    );

    void Term();

    DescriptorHandle* GetHandleRTV() const;
    ID3D12Resource* GetResource() const;
    D3D12_RESOURCE_DESC GetDesc() const;
    D3D12_RENDER_TARGET_VIEW_DESC GetViewDesc() const;

private:

    ComPtr<ID3D12Resource>          m_pTarget;
    DescriptorHandle* m_pHandleRTV;
    DescriptorHeap* m_pHeapRTV;
    D3D12_RENDER_TARGET_VIEW_DESC   m_ViewDesc;
};

}//namespace Graphics
}//namespace Lib