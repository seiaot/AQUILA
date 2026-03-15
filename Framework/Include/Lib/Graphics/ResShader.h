#pragma once

#include <d3d12.h>
#include <string>

#include "ComPtr.h"

namespace Lib {
namespace Graphics {

class ResShader
{
    ResShader(const ResShader&) = delete;
    void operator = (const ResShader&) = delete;

public:

    ResShader();
    ~ResShader();

    bool Init(const wchar_t* pFilePath);
    void Term();

    ID3DBlob* Get();

private:

    ComPtr<ID3DBlob> m_pBlob;
    std::wstring m_FilePath;
};

}//namespace Graphics
}//namespace Lib
