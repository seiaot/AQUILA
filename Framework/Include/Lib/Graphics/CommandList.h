#pragma once

#include <d3d12.h>
#include <cstdint>
#include <vector>

#include "ComPtr.h"

namespace Lib{
namespace Graphics{

class CommandList
{
    CommandList(const CommandList&) = delete;
    void operator = (const CommandList&) = delete;

public:
    CommandList();
    ~CommandList();

    bool Init(
        ID3D12Device* pDevice,
        D3D12_COMMAND_LIST_TYPE type,
        uint32_t framecount
    );

    void Term();

    void Reset(uint32_t index);
    ID3D12GraphicsCommandList* Get()const;

    void SetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation);
    void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);

private:

    ComPtr<ID3D12GraphicsCommandList>           m_pCmdList;
    std::vector<ComPtr<ID3D12CommandAllocator>> m_pAllocators;

    D3D12_GPU_DESCRIPTOR_HANDLE m_currentGpuDescriptorHandle[16];
};

}//namespace Graphics
}//namespace Lib