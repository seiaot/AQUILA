#include "Lib/Graphics/CommandList.h"

namespace Lib {
namespace Graphics {

CommandList::CommandList()
    : m_pCmdList(nullptr)
    , m_pAllocators()
    , m_currentGpuDescriptorHandle()
{
    ;
}

CommandList::~CommandList()
{
    Term();
}

bool CommandList::Init(
    ID3D12Device* pDevice,
    D3D12_COMMAND_LIST_TYPE type,
    uint32_t framecount
) {
    if (pDevice == nullptr || framecount == 0)
        return false;

    m_pAllocators.resize(framecount);

    for (auto i = 0u; i < framecount; ++i)
    {
        auto hr = pDevice->CreateCommandAllocator(
            type,
            IID_PPV_ARGS(m_pAllocators[i].GetAddressOf())
        );

        if (FAILED(hr))
            return false;
    }

    //コマンドリストの初期化。最初はクローズ状態にしておく
    {
        auto hr = pDevice->CreateCommandList(
            1,
            type,
            m_pAllocators[0].Get(),
            nullptr,
            IID_PPV_ARGS(m_pCmdList.GetAddressOf())
        );

        if (FAILED(hr))
            return false;

        m_pCmdList->Close();
    }

    for (int i = 0; i < 16; ++i) {
        m_currentGpuDescriptorHandle[i].ptr = 0;
    }

    return true;
}

void CommandList::Term()
{
    m_pCmdList.Reset();

    for (size_t i = 0; i < m_pAllocators.size(); ++i)
    {
        m_pAllocators[i].Reset();
    }

    m_pAllocators.clear();
    m_pAllocators.shrink_to_fit();
}

void CommandList::Reset(uint32_t index)
{
    auto hr = m_pAllocators[index]->Reset();

    for (int i = 0; i < 16; ++i) {
        m_currentGpuDescriptorHandle[i].ptr = 0;
    }

    if (FAILED(hr))
        return ;

    hr = m_pCmdList->Reset(m_pAllocators[index].Get(), nullptr);

    if (FAILED(hr))
        return ;    


}

ID3D12GraphicsCommandList* CommandList::Get()const {
    return m_pCmdList.Get();
}

void CommandList::SetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) {
    m_pCmdList.Get()->SetGraphicsRootConstantBufferView(RootParameterIndex, BufferLocation);
}

void CommandList::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) {

    if (RootParameterIndex < 16 && m_currentGpuDescriptorHandle[RootParameterIndex].ptr == BaseDescriptor.ptr) {
        return;
    }

    m_pCmdList.Get()->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);

    if (RootParameterIndex < 16) {
        m_currentGpuDescriptorHandle[RootParameterIndex] = BaseDescriptor;
    }
}

}//namespace Graphics
}//namespace Lib