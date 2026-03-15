#pragma once

//平面ポリゴン

#include <d3d12.h>
#include <dxgi.h>

#include "Shape.h"
#include "Lib/Graphics/MeshIB.h"
#include "Lib/Graphics/ResMesh.h"

using namespace Lib::Graphics;

namespace Fwk{

class ShapePlane : public Shape
{
public:
	ShapePlane();
	~ShapePlane();

	void Init(ID3D12Device* pDevice) override;
	void Term() override;
	void Draw(ID3D12GraphicsCommandList* pCmdList) override;

	void SetTexCoord(const XMFLOAT2& left_top, const XMFLOAT2& right_bottom);
	void SetColor(float r, float g, float b, float a);
	void SetColor(float r, float g, float b);
	void SetAlpha(float a);

private:
	MeshIB m_Mesh;
	MeshVertex* m_pVertex;
};

}//namespace Fwk