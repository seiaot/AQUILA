#pragma once

#include <d3d12.h>

#include "D3D.h"
#include "Lib/Graphics/ComPtr.h"

class CommandQueue
{
public:

	CommandQueue();
	~CommandQueue();

	bool Init(ID3D12Device* pDevice);
	void Term();

	ID3D12CommandQueue* Get();

private:

	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
};

