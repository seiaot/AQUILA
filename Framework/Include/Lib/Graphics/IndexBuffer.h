#pragma once

#include <d3d12.h>
#include <cstdint>

#include "ComPtr.h"

namespace Lib {
namespace Graphics {

class IndexBuffer
{
    IndexBuffer(const IndexBuffer&) = delete;
    void operator = (const IndexBuffer&) = delete;

public:

    IndexBuffer();
    ~IndexBuffer();

    bool Init(ID3D12Device* pDevice, size_t size, const uint32_t* pInitData = nullptr);

    void Term();

    uint32_t* Map();
    void Unmap();

    D3D12_INDEX_BUFFER_VIEW GetView() const;

private:

    ComPtr<ID3D12Resource>      m_pIB;
    D3D12_INDEX_BUFFER_VIEW     m_View;
};

}//namespace Graphics
}//namespace Lib