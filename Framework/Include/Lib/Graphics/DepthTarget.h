#pragma once

#include <d3d12.h>
#include <cstdint>
#include "ComPtr.h"

namespace Lib{
namespace Graphics {

//前方宣言
class DescriptorHandle;
class DescriptorHeap;

class DepthTarget
{
    DepthTarget(const DepthTarget&) = delete;
    void operator = (const DepthTarget&) = delete;

public:

    DepthTarget();
    ~DepthTarget();

    bool Init(
        ID3D12Device* pDevice,
        DescriptorHeap* pHeapDSV,
        uint32_t        width,
        uint32_t        height
    );

    void Term();
    DescriptorHandle* GetHandleDSV() const;
    ID3D12Resource* GetResource() const;
    D3D12_RESOURCE_DESC GetDesc() const;
    D3D12_DEPTH_STENCIL_VIEW_DESC GetViewDesc() const;

private:

    ComPtr<ID3D12Resource>          m_pResTarget;
    DescriptorHandle* m_pHandleDSV;
    DescriptorHeap* m_pHeapDSV;
    D3D12_DEPTH_STENCIL_VIEW_DESC   m_ViewDesc;
};

}//namespace Graphics
}//namespace Lib