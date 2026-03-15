#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

using namespace DirectX;
using namespace std;

namespace Lib {
namespace Graphics {

struct MeshVertex {
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent;
	XMFLOAT3 AddColor;

	MeshVertex() = default;

	MeshVertex(
		const XMFLOAT3& _position,
		const XMFLOAT3& _normal,
		const XMFLOAT4& _color,
		const XMFLOAT2& _texcoord,
		const XMFLOAT3& _tangent,
		const XMFLOAT3& _addColor
	)
		: Position(_position)
		, Normal(_normal)
		, Color(_color)
		, TexCoord(_texcoord)
		, Tangent(_tangent)
		, AddColor(_addColor)
	{
		;
	}

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 6;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct ResMaterial
{
	XMFLOAT3 Diffuse;
	XMFLOAT3 Specular;
	float Alpha;
	float Shininess;
	string DiffuseMap;
};

struct ResMesh {
	vector<MeshVertex> Vertices;
	vector<uint32_t> Indices;
	uint32_t MaterialId;
};

bool LoadMesh(
	const wchar_t* filename,
	vector<ResMesh>& meshes,
	vector<ResMaterial>& materials
);

}//namespace Graphics
}//namespace Lib