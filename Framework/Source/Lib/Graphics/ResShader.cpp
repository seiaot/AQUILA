#include "Lib/Graphics/ResShader.h"

#include <d3dcompiler.h>

#include "Lib/Util/FileUtil.h"

#pragma comment( lib, "d3dcompiler.lib" )

namespace Lib {
namespace Graphics {

ResShader::ResShader()
    : m_pBlob(nullptr)
    , m_FilePath(L"")
{
    ;
}

ResShader::~ResShader()
{
    Term();
}

bool ResShader::Init(const wchar_t* pFilePath)
{
    if (!PathFileExistsW(pFilePath))
        return false;

    m_FilePath = pFilePath;

    //頂点シェーダの読み込み
    auto hr = D3DReadFileToBlob(m_FilePath.c_str(), m_pBlob.GetAddressOf());

    if (FAILED(hr))
        return false;

    // 正常終了.
    return true;
}

void ResShader::Term()
{
    m_pBlob.Reset();
    m_FilePath = L"";
}

ID3DBlob* ResShader::Get() {
    return m_pBlob.Get();
}

}//namespace Graphics
}//namespace Lib