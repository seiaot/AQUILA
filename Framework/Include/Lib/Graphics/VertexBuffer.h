#pragma once
#include <d3d12.h>
#include "ComPtr.h"

namespace Lib {
namespace Graphics {

class VertexBuffer
{
    VertexBuffer(const VertexBuffer&) = delete;
    void operator = (const VertexBuffer&) = delete;

public:

    VertexBuffer();

    ~VertexBuffer();

    bool Init(ID3D12Device* pDevice, size_t size, size_t stride, const void* pInitData = nullptr);

    template<typename T>
    bool Init(ID3D12Device* pDevice, size_t size, const T* pInitData = nullptr)
    {
        return Init(pDevice, size, sizeof(T), pInitData);
    }

    void Term();

    void* Map();

    void Unmap();

    template<typename T>
    T* Map() 
    {
        return reinterpret_cast<T*>(Map());
    }

    D3D12_VERTEX_BUFFER_VIEW GetView() const;

private:
    ComPtr<ID3D12Resource>      m_pVB;
    D3D12_VERTEX_BUFFER_VIEW    m_View;
};

}//namespace Graphics
}//namespace Lib