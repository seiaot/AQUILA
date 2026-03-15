#include "Lib/Graphics/DepthTarget.h"
#include "Lib/Graphics/DescriptorHeap.h"

namespace Lib{
namespace Graphics {

DepthTarget::DepthTarget()
    : m_pResTarget(nullptr)
    , m_pHandleDSV(nullptr)
    , m_pHeapDSV(nullptr)
    , m_ViewDesc()
{
    ;
}

DepthTarget::~DepthTarget()
{
    Term();
}

bool DepthTarget::Init
(
    ID3D12Device* pDevice,
    DescriptorHeap* pHeapRTV,
    uint32_t        width,
    uint32_t        height
)
{
    if (pDevice == nullptr || pHeapRTV == nullptr || width == 0 || height == 0)
    {
        return false;
    }

    assert(m_pHandleDSV == nullptr);
    assert(m_pHeapDSV == nullptr);

    m_pHeapDSV = pHeapRTV;
    m_pHeapDSV->AddRef();

    m_pHandleDSV = m_pHeapDSV->AllocHandle();
    if (m_pHandleDSV == nullptr)
    {
        return false;
    }

    D3D12_HEAP_PROPERTIES prop = {};
    prop.Type = D3D12_HEAP_TYPE_DEFAULT;      //CPUからアクセスせずGPUから頻繁にアクセスするので
                                                                //デフォルト設定とする
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = UINT64(width);
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    auto hr = pDevice->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(m_pResTarget.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    m_ViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    m_ViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    m_ViewDesc.Texture2D.MipSlice = 0;
    m_ViewDesc.Flags = D3D12_DSV_FLAG_NONE;

    pDevice->CreateDepthStencilView(
        m_pResTarget.Get(),
        &m_ViewDesc,
        m_pHandleDSV->HandleCPU
    );

    return true;
}

void DepthTarget::Term()
{
    m_pResTarget.Reset();

    if (m_pHeapDSV != nullptr && m_pHandleDSV != nullptr)
    {
        m_pHeapDSV->FreeHandle(m_pHandleDSV);
        m_pHandleDSV = nullptr;
    }

    if (m_pHeapDSV != nullptr)
    {
        m_pHeapDSV->Release();
        m_pHeapDSV = nullptr;
    }
}

DescriptorHandle* DepthTarget::GetHandleDSV() const
{
    return m_pHandleDSV;
}

ID3D12Resource* DepthTarget::GetResource() const
{
    return m_pResTarget.Get();
}

D3D12_RESOURCE_DESC DepthTarget::GetDesc() const
{
    if (m_pResTarget == nullptr)
    {
        return D3D12_RESOURCE_DESC();
    }

    return m_pResTarget->GetDesc();
}

D3D12_DEPTH_STENCIL_VIEW_DESC DepthTarget::GetViewDesc() const
{
    return m_ViewDesc;
}

}//namespace Graphics
}//namespace Lib