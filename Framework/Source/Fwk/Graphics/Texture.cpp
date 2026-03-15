#include "Fwk/Graphics/Texture.h"

#include "Fwk/Framework.h"
#include "Fwk/Asset/AssetTexture.h"

Texture::Texture() 
: m_Texture()
{
	;
}
Texture::~Texture() {
	Unload();
}

void Texture::Load(const char* filename) {

	wchar_t wcharBuffer[256] = { 0 };
	int wcharNum = MultiByteToWideChar(CP_UTF8, 0, filename, (int)strlen(filename), nullptr, 0);
	if (wcharNum == 0 || wcharNum >= 255) {
		return;
	}

	MultiByteToWideChar(CP_UTF8, 0, filename, (int)strlen(filename), wcharBuffer, wcharNum);
	Load(wcharBuffer);
}

void Texture::Load(const wchar_t* filename)
{
	Unload();
	AssetManager_I->LoadAsset<Fwk::AssetTexture>(filename, &m_Texture);
}

void Texture::Load(const std::string& filename) {
	Load(filename.c_str());
}

void Texture::Load(const std::wstring& filename) {
	Load(filename.c_str());
}

void Texture::Unload() {
	auto fwk = Framework_I;
	if (fwk == nullptr)
		return;

	AssetManager_I->Unload(&m_Texture);
}
TextureSRV* Texture::GetSRV()const {

	auto pTex = m_Texture.Get< Fwk::AssetTexture>();
	if (pTex == nullptr)
		return nullptr;

	return pTex->GetSRV();
}
UINT64 Texture::GetWidth()const {

	auto pTex = m_Texture.Get< Fwk::AssetTexture>();
	if (pTex == nullptr)
		return 0;

	return pTex->GetWidth();
}

INT Texture::GetHeight()const {
	auto pTex = m_Texture.Get< Fwk::AssetTexture>();
	if (pTex == nullptr)
		return 0;

	return pTex->GetHeight();
}