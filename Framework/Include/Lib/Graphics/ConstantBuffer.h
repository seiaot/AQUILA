#pragma once

#include <d3d12.h>
#include <vector>
#include "ComPtr.h"

namespace Lib{
namespace Graphics {

class DescriptorHandle;
class DescriptorHeap;

class ConstantBuffer
{
    ConstantBuffer(const ConstantBuffer&) = delete;
    void operator = (const ConstantBuffer&) = delete;

public:
    ConstantBuffer();
    ~ConstantBuffer();

    bool Init(
        ID3D12Device* pDevice,
        DescriptorHeap* pHeap,
        size_t          size);

    void Term();

    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

    void* GetPtr() const;

    template<typename T>
    T* GetPtr()
    {
        return reinterpret_cast<T*>(GetPtr());
    }

private:

    ComPtr<ID3D12Resource>          m_pCB;
    DescriptorHandle* m_pHandle;
    DescriptorHeap* m_pHeap;
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc;
    void* m_pMappedPtr;
};

}//namespace Graphics
}//namespace Lib