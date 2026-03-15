#pragma once

#include <map>

#include "DescriptorHeap.h"
#include "ResourceUploadBatch.h"
#include "TextureSRV.h"
#include "ConstantBuffer.h"


namespace Lib
{
namespace Graphics
{

class Material
{

public:

    enum TEXTURE_USAGE
    {
        TEXTURE_USAGE_DIFFUSE = 0,  //!< ディフューズマップとして利用します.
        TEXTURE_USAGE_SPECULAR,     //!< スペキュラーマップとして利用します.
        TEXTURE_USAGE_SHININESS,    //!< シャイネスマップとして利用します.
        TEXTURE_USAGE_NORMAL,       //!< 法線マップとして利用します.

        TEXTURE_USAGE_COUNT
    };

    Material();
    ~Material();

    bool Init(
        ID3D12Device* pDevice,
        DescriptorHeap* pPool,
        size_t          bufferSize,
        size_t          count);

    void Term();

    bool SetTexture(
        size_t                          index,
        TEXTURE_USAGE                   usage,
        ID3D12CommandQueue*             pCommandQueue,
        const std::wstring& path);

    void* GetBufferPtr(size_t index) const;

    template<typename T>
    T* GetBufferPtr(size_t index) const
    {
        return reinterpret_cast<T*>(GetBufferPtr(index));
    }

    D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(size_t index) const;

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(size_t index, TEXTURE_USAGE usage) const;

    size_t GetCount() const;

private:

    struct Subset
    {
        ConstantBuffer* pCostantBuffer;                     //!< 定数バッファです.
        D3D12_GPU_DESCRIPTOR_HANDLE     TextureHandle[TEXTURE_USAGE_COUNT]; //!< テクスチャハンドルです.
    };

    std::map<std::wstring, TextureSRV*>    m_pTexture;     //!< テクスチャです.
    std::vector<Subset>                 m_Subset;       //!< サブセットです.
    ID3D12Device* m_pDevice;      //!< デバイスです.
    DescriptorHeap* m_pPool;        //!< ディスクリプタプールです(CBV_UAV_SRV).

    //=========================================================================
    // private methods.
    //=========================================================================
    Material(const Material&) = delete;
    void operator = (const Material&) = delete;
};

constexpr auto TU_DIFFUSE = Material::TEXTURE_USAGE_DIFFUSE;
constexpr auto TU_SPECULAR = Material::TEXTURE_USAGE_SPECULAR;
constexpr auto TU_SHININESS = Material::TEXTURE_USAGE_SHININESS;
constexpr auto TU_NORMAL = Material::TEXTURE_USAGE_NORMAL;

}
}