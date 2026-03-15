// ----------------------------------------------------
// テクスチャアセットクラス
// 
// 
// 
// ----------------------------------------------------
#include "Fwk/Asset/AssetTexture.h"

#include "Lib/Graphics//Graphics.h"
#include "Lib/Util/Log.h"

#include "Fwk/Framework.h"

using namespace Lib::Graphics;

namespace Fwk
{

AssetTexture::AssetTexture()
: m_Width(0)
, m_Height(0)
{
	;
}

AssetTexture::~AssetTexture()
{
	Term();
}

bool AssetTexture::Init(const wchar_t* pFilePath) {

	IGraphics * pGraphics = RenderManager_I->GetGraphics();

	Debug::Logf("テクスチャの読み込み開始:%ls",pFilePath);

	//テクスチャの生成
	bool bResult = m_TextureSRV.Init(
		pGraphics->GetDevice(),
		pGraphics->GetCommandQueue(),
		pGraphics->GetDescriptorHeap(Lib::Graphics::HEAP_TYPE_RES),
		pFilePath
	);

	if (bResult) {
		Debug::Log(" ...成功\n");
	}
	else {
		Debug::Log(" ...失敗\n");
		Debug::Logf("エラー：テクスチャの読み込みに失敗しました。%ls\n",pFilePath);

		return false;
	}

	D3D12_RESOURCE_DESC desc = {};

	m_TextureSRV.GetDesc(&desc);

	m_Width = desc.Width;
	m_Height = desc.Height;

	return true;
}
void AssetTexture::OnTerm() {
	
	m_TextureSRV.Term();
	m_Width = 0;
	m_Height = 0;
}

TextureSRV* AssetTexture::GetSRV() {
	return &m_TextureSRV;
}

UINT64 AssetTexture::GetWidth() const{
	return m_Width;
}

INT AssetTexture::GetHeight() const{
	return m_Height;
}


}