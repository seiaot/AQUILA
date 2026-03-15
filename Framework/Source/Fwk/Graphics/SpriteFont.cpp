#include "Fwk/Graphics/SpriteFont.h"

#include "Lib/Graphics/Graphics.h"
#include "Lib/Graphics/DescriptorHeap.h"
#include "Lib/LibDef.h"
#include "Lib/Util/FileUtil.h"

//dxtk -------------------------------
#pragma comment( lib, "DirectXTK12.lib" )
#pragma comment( lib, "dxguid.lib" )
//------------------------------------

namespace Fwk {

SpriteFont::SpriteFont()
	: m_pGraphicsMemory(nullptr)
	, m_pSpriteFont(nullptr)
	, m_pSpriteBatch(nullptr)
	, m_pHeapSRV(nullptr)
	, m_pHeapHandle(nullptr)
{
	;
}

SpriteFont::~SpriteFont()
{
	Term();
}

bool SpriteFont::Init(
	Lib::Graphics::IGraphics* g,
	const wchar_t* pFontFileName
)
{
	if (g == nullptr || g->GetDevice() == nullptr)
		return false;

    if (!PathFileExistsW(pFontFileName))
        return false;

	Term();

	m_pGraphicsMemory = new DirectX::GraphicsMemory(g->GetDevice());

	DirectX::ResourceUploadBatch resUploadBatch(g->GetDevice());
	resUploadBatch.Begin();
	DirectX::RenderTargetState rtState(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_D32_FLOAT
	);

	DirectX::SpriteBatchPipelineStateDescription pd(rtState);

	m_pSpriteBatch = new DirectX::SpriteBatch(g->GetDevice(), resUploadBatch, pd);

	m_pHeapSRV = g->GetDescriptorHeap(Lib::Graphics::HEAP_TYPE_RES);
	m_pHeapSRV->AddRef();

	m_pHeapHandle = m_pHeapSRV->AllocHandle();

	m_pSpriteFont = new DirectX::SpriteFont
	(
		g->GetDevice(),
		resUploadBatch,
		pFontFileName,
		m_pHeapHandle->HandleCPU,
		m_pHeapHandle->HandleGPU
	);

	auto future = resUploadBatch.End(g->GetCommandQueue());
	future.wait();

	return true;
}

void SpriteFont::Term()
{
	SAFE_DELETE(m_pSpriteFont);

	if(m_pHeapHandle && m_pHeapSRV)
	{
		m_pHeapSRV->FreeHandle(m_pHeapHandle);
		m_pHeapHandle = nullptr;
	}

	if (m_pHeapSRV) {
		m_pHeapSRV->Release();
		m_pHeapSRV = nullptr;
	}
	
	SAFE_DELETE(m_pSpriteBatch);
	SAFE_DELETE(m_pGraphicsMemory);
}

void SpriteFont::BeginRender(Lib::Graphics::IGraphics* pGraphics) {
	if (pGraphics == nullptr )
		return;
	
	if (m_pSpriteBatch == nullptr)
		return;

	if (m_pHeapSRV == nullptr)
		return;


	ID3D12DescriptorHeap* const pHeaps[] = {
		m_pHeapSRV->Get()
	};

	pGraphics->GetCommandList()->Get()->SetDescriptorHeaps(1, pHeaps);

	m_pSpriteBatch->SetViewport(pGraphics->GetViewPort());
	m_pSpriteBatch->Begin(
		pGraphics->GetCommandList()->Get(),
		DirectX::SpriteSortMode_BackToFront
	);
}

void SpriteFont::EndRender() {

	if (m_pSpriteBatch == nullptr)
		return;
	
	m_pSpriteBatch->End();
}

void SpriteFont::PostPresent(Lib::Graphics::IGraphics* pGraphics) {

	if (pGraphics == nullptr)
		return;

	if (m_pGraphicsMemory == nullptr)
		return;

	m_pGraphicsMemory->Commit(pGraphics->GetCommandQueue());
}

void SpriteFont::Draw(
	const char* pText,
	const DirectX::XMFLOAT2& location,
	const DirectX::XMFLOAT4& color,
	float rotation,
	const DirectX::XMFLOAT2& origin,
	const DirectX::XMFLOAT2& scale,
	float depth
) 
{
	DirectX::XMVECTOR _location = DirectX::XMLoadFloat2(&location);
	DirectX::XMVECTOR _color = DirectX::XMLoadFloat4(&color);
	DirectX::XMVECTOR _origin = DirectX::XMLoadFloat2(&origin);
	DirectX::XMVECTOR _scale = DirectX::XMLoadFloat2(&scale);

	m_pSpriteFont->DrawString
	(
		m_pSpriteBatch
		,pText
		,_location
		, _color
		,rotation
		,_origin
		, _scale
		, DirectX::SpriteEffects_None
		, depth
	);
}

void SpriteFont::Draw(
	const wchar_t* pText,
	const DirectX::XMFLOAT2& location,
	const DirectX::XMFLOAT4& color,
	float rotation,
	const DirectX::XMFLOAT2& origin,
	const DirectX::XMFLOAT2& scale,
	float depth
)
{
	DirectX::XMVECTOR _location = DirectX::XMLoadFloat2(&location);
	DirectX::XMVECTOR _color = DirectX::XMLoadFloat4(&color);
	DirectX::XMVECTOR _origin = DirectX::XMLoadFloat2(&origin);
	DirectX::XMVECTOR _scale = DirectX::XMLoadFloat2(&scale);

	m_pSpriteFont->DrawString
	(
		m_pSpriteBatch
		, pText
		, _location
		, _color
		, rotation
		, _origin
		, _scale
		, DirectX::SpriteEffects_None
		, depth
	);
}

}//namespace Fwk