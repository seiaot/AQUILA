#include "Lib/Graphics/CommandQueue.h"

CommandQueue::CommandQueue()
{
}

CommandQueue::~CommandQueue()
{
}

bool CommandQueue::Init(ID3D12Device* pDevice)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	auto hr = pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pCommandQueue.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void CommandQueue::Term()
{
	m_pCommandQueue.Reset();
}

ID3D12CommandQueue* CommandQueue::Get()
{
	return m_pCommandQueue.Get();
}
